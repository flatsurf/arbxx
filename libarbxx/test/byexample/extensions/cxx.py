r"""
Extend byexample with natural C(++) tests

Byexample supports C++ directly through cling. However, it expects tests to be
written in cling syntax which is somewhat cryptic to readers not familiar with
cling. Also, pexpect style parsing is not terribly robust.

Here we extend byexample with a parser that can detect regular source code in
Markdown code fences and expected outputs in `// ->` comments.

We also use cppyy instead of cling which takes care of the hard part of the
parsing for us.
"""
######################################################################
#  This file is part of arbxx.
#
#        Copyright (C) 2021-2022 Julian Rüth
#
#  arbxx is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or (at your
#  option) any later version.
#
#  arbxx is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with arbxx. If not, see <https://www.gnu.org/licenses/>.
#####################################################################
from __future__ import unicode_literals
import re
from byexample.finder import ZoneDelimiter
from byexample.common import constant
import byexample.modules.cpp

stability = 'experimental'


class MarkdownCxxDelimiter(ZoneDelimiter):
    r"""
    Detects C/C++ examples in Markdown.
    """
    target = {'.md'}

    @constant
    def zone_regex(self):
        return re.compile(r'''
            # Begin with a Markdown fenced-code marker
            ^[ ]*
                (?:
                    (?P<marker>```(?:``)*)[ ]*(c|c\+\+|cpp)\b # fenced-code marker (backticks + language)
                )
            # then, grab everything until the first end marker
            (?P<zone>.*?)
            # finally, the end marker
            (?(marker)
                  ^[ ]*(?P=marker) # we must match the same amount of backticks
            )
            ''', re.DOTALL | re.MULTILINE | re.VERBOSE)

    def __repr__(self): return "```c++ ... ``` or ```cpp ... ``` or ```c ... ```"


class MarkdownHppDelimiter(ZoneDelimiter):
    r"""
    Detect C/C++ examples in Markdown comments in header files like the ones
    that we are using for standardese.

    TESTS::

    >>> re = MarkdownHppDelimiter(verbosity=0, encoding=None).zone_regex()
    >>> re.findall(r'''
    ...     ///
    ...     ///     DEMO
    ...     ///     DEMO
    ...     ///
    ...     ///     DEMO
    ...     ///
    ... ''')
    [('    ', '    ///     DEMO\n    ///     DEMO\n'), ('    ', '    ///     DEMO\n')]


    """
    target = {'.hpp'}

    @constant
    def zone_regex(self):
        return re.compile(r'''
            # Begin with an empty line.
            (?:^(?P<indent>[ ]*)///[ ]?$)\n
            # Then grab every indented code block.
            (?P<zone>(?:(?P=indent)///[ ][ ]{4}[^\n]*$\n)+)
            ''', re.DOTALL | re.MULTILINE | re.VERBOSE)

    def __repr__(self): return "///\n///    ..."


class CxxPromptFinder(byexample.modules.cpp.CppPromptFinder):
    r"""
    Detect blocks of C++ code delimited by a `// ->` comment that provides the expected output.
    """
    target = 'cpp-prompt'

    @constant
    def example_regex(self):
        return re.compile(r'''
            (?P<snippet>
                (?:^                                # match the first line
                  (?P<indent> [ ]* (?:///[ ])?[ ]*) # arbitrary indent, captured so we can strip it later
                  [^\n]+                            # followed by anything on the same line
                )
                (?:\n
                  (?P=indent) (?!//[ ]->[ ]) [^\n]* # same on the following lines until we run into an output "// -> "
                )*
            )
            \n?
            (?P<expected>
              (?:\n?
                (?P=indent)                         # the expected output starts with the indent
                (?: //[ ]->[ ] )                    # followed by the // -> marker
                [^\n]*                              # followed by the expected output on one line
              )*
            )
            \n?
            (?:[ ]* (?:///[ ]*)?\n)*                # finally, consume empty lines to prevent confusing indent detection of next block
            ''', re.MULTILINE | re.VERBOSE)

    def get_snippet_and_expected(self, match, where):
        snippet, expected = super(CxxPromptFinder, self).get_snippet_and_expected(match, where)
        expected = self._remove_expected(expected)

        return snippet, expected

    def _remove_prompts(self, snippet):
        return snippet

    def _remove_expected(self, snippet):
        marker = "// -> "
        lines = snippet.split("\n")
        lines = [line[len(marker):] if line.startswith(marker) else line for line in lines]
        return '\n'.join(lines)

    def __repr__(self): return "Unprefixed C++ Prompt Finder"


class CxxInterpreter:
    def __init__(self, connection):
        self._connection = connection

        self.loop()

    def loop(self):
        while True:
            self.stdout = ""
            self.stderr = ""

            try:
                code = self.receive_command()
            except EOFError:
                return

            try:
                self.run(code)
            except Exception as e:
                self.report(e)
            else:
                self.report()

    def receive_command(self):
        code, = self._connection.recv()
        return code

    def run(self, code):
        definitions, executables = self.parse(code)

        for definition in definitions:
            self.define(definition)

        for executable in executables:
            self.execute(executable)

    def parse(self, code):
        lines = code.split('\n')
        definitions = [line for line in lines if line.startswith('#include')]
        definitions.append("#include <iostream>")

        executables = ["std::cout << std::setprecision(6);"] + [line for line in lines if not line.startswith('#include') and line]

        if executables and not executables[-1].endswith(';'):
            executables[-1] = f"std::cout << std::boolalpha << ({executables[-1]});"

        if executables:
            # name lookup gets confused if we don't run the entire executable block at once
            executables = ["\n".join(executables)]

        return definitions, executables

    def report(self, exception=None):
        self._connection.send((self.stdout, self.stderr, exception))

    def run_captured(self, callback):
        import py.io

        capture = py.io.StdCaptureFD()
        try:
            try:
                callback()
            finally:
                self.record(*capture.reset())
        except Exception as e:
            raise self.create_exception(e)

    def record(self, stdout, stderr):
        if self.stdout and stdout:
            self.stdout += '\n'
        self.stdout += stdout

        if self.stderr and stderr:
            self.stderr += '\n'
        self.stderr += stderr

    def define(self, definition):
        import cppyy
        return self.run_captured(lambda: cppyy.cppdef(definition))

    def execute(self, executable):
        import cppyy
        import cppyy.ll

        # work around https://github.com/wlav/cppyy/issues/32
        cppyy.ll.set_signals_as_exception(True)
        try:
            self.run_captured(lambda: cppyy.cppexec(executable))
        finally:
            cppyy.ll.set_signals_as_exception(False)

    def create_exception(self, exception):
        if isinstance(exception, SyntaxError):
            # cppyy is throwing a SyntaxError even though the syntax is fine, see https://github.com/wlav/cppyy/issues/32
            import re
            match = re.search("^terminate called after throwing an instance of '([^']*)'\n  what\\(\\):  (.*)\n Generating stack trace\\.\\.\\.", self.stderr)

            if match:
                self.stderr = ""

                exception = match.group(1)
                what = match.group(2)

                if exception == "std::invalid_argument":
                    exception = ValueError(what)
                else:
                    exception = Exception(what)

        import cppyy
        if isinstance(exception, cppyy.gbl.std.invalid_argument):
            exception = ValueError(exception.what())

        if isinstance(exception, cppyy.gbl.std.exception):
            exception = Exception(exception.what())

        raise exception


class CxxRunner(byexample.runner.ExampleRunner):
    r"""
    Run cppyy.cppexec on code samples.
    """
    language = 'cpp'

    def __repr__(self): return "C++ Interpreter"

    def initialize(self, options):
        import os.path
        import sys
        sys.path.append(os.path.dirname(__file__))

        from multiprocessing import Pipe
        self._parent_connection, child_connection = Pipe()

        from multiprocessing import Process
        self._child = Process(target=CxxInterpreter, args=(child_connection,))
        self._child.start()

    def run(self, example, options):
        self._parent_connection.send((example.source,))

        try:
            stdout, stderr, exception = self._parent_connection.recv()
        except Exception as e:
            raise Exception(f"Interpreter crashed while evaluating {example}", e)

        verdict = ""

        if stderr:
            if verdict:
                verdict += '\n'
            verdict += stderr
        if stdout:
            if verdict:
                verdict += '\n'
            verdict += stdout
        if exception:
            if verdict:
                verdict += '\n'
            verdict += str(exception)

        return verdict

    def shutdown(self):
        self._parent_connection.close()
        self._child.join(1)
        if self._child.exitcode is None:
            self._child.kill()
