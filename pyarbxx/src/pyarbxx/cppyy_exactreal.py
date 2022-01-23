r"""
Python wrappers for arb powered by cppyy.

Note that pyarbxx offers a better interface to work with arb. Use
`pyarbxx.arbxx` if you need to directly manipulate the underlying C++
objects::

>>> from pyarbxx import arbxx
>>> arbxx.Module[arbxx.IntegerRing].make([])
ℤ-Module()

"""
# ********************************************************************
#  This file is part of arbxx.
#
#        Copyright (C) 2019-2022 Julian Rüth
#
#  arbxx is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  arbxx is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with arbxx. If not, see <https://www.gnu.org/licenses/>.
# ********************************************************************

import cppyy
import gmpxxyy

from cppyythonizations.pickling.cereal import enable_cereal
from cppyythonizations.printing import enable_pretty_printing

class PrecisionError(ArithmeticError):
    r"""
    Raised when the result of an operation can not be decided in ball arithmetic.

    EXAMPLES::

        >>> from pyarbxx import arbxx
        >>> import cppyy
        >>> a = arbxx.Arb(cppyy.gbl.mpq_class(1, 3), 64); a
        [0.333333 +/- 3.34e-7]
        >>> a == 1
        False
        >>> a == a
        Traceback (most recent call last):
        ...
        pyarbxx.cppyy_arbxx.PrecisionError: ...

    """

# Importing cysignals after cppyy gives us proper stack traces on segfaults
# whereas cppyy otherwise only reports "segmentation violation" (which is
# probably what cling provides.)
import os
if os.environ.get('PYEXACTREAL_CYSIGNALS', True):
    try:
        import cysignals
    except ModuleNotFoundError:
        pass

def enable_arithmetic(proxy, name):
    r"""
    Help cppyy discover arithmetic provided by boost::operators.

    TESTS::

        >>> from pyarbxx import ZZModule, RealNumber
        >>> M = ZZModule(RealNumber.rational(1))
        >>> x = M.gen(0)
        >>> x + x
        2
        >>> x - x
        0
        >>> x * x
        1
        >>> x / x
        1

    """
    for op in ['add', 'sub', 'mul', 'truediv']:
        python_op = "__%s__" % (op,)
        python_rop = "__r%s__" % (op,)

        def unwrap_binary_optional(x):
            if not hasattr(x, 'has_value'):
                return x
            if not x.has_value():
                # e.g., when a division failed because x/y does not live in the coefficient ring
                raise NotRepresentableError("result is not representable in this parent")
            return x.value()

        implementation = getattr(cppyy.gbl.arbxx.cppyy, op)
        def binary(lhs, rhs, implementation=implementation):
            return unwrap_binary_optional(implementation[type(lhs), type(rhs)](lhs, rhs))
        def rbinary(rhs, lhs, implementation=implementation):
            return unwrap_binary_optional(implementation[type(lhs), type(rhs)](lhs, rhs))

        setattr(proxy, python_op, binary)
        setattr(proxy, python_rop, rbinary)

    setattr(proxy, "__neg__", lambda self: cppyy.gbl.arbxx.cppyy.neg(self))

arithmetic_predicate = lambda proxy, name: name in ["Mag", "Arf", "Arb", "Acb", "ArbPoly", "AcbPoly", "ArbMat", "AcbMat"]

def add_pythonization(pythonization, namespace, predicate=lambda *args: True):
    r"""
    Conditionally enable a pythonization on a namespace.
    """
    cppyy.py.add_pythonization(lambda proxy, name: predicate(proxy, name) and pythonization(proxy, name), namespace)

add_pythonization(enable_arithmetic, "arbxx", arithmetic_predicate)

def enable_optional(proxy, name):
    if name in  ["Arb"]:
        def unwrap_logical_optional(x):
            if not hasattr(x, 'has_value'):
                return x
            if not x.has_value():
                raise PrecisionError("precision insufficient to decide this relation or not well defined for arguments")
            return x.value()

        for op in ["lt", "le", "eq", "ne", "gt", "ge"]:
            python_op = "__%s__"%(op,)
            hidden_op = "_cppyy_%s"%(op,)
            setattr(proxy, hidden_op, getattr(proxy, python_op))
            def relation(lhs, rhs, op=hidden_op):
                return unwrap_logical_optional(getattr(lhs, op)(rhs))
            setattr(proxy, python_op, relation)

cppyy.py.add_pythonization(enable_optional, "arbxx")

cppyy.py.add_pythonization(enable_pretty_printing, "arbxx")

cppyy.py.add_pythonization(lambda proxy, name: enable_cereal(proxy, name, ["arbxx/cereal.hpp"]), "arbxx")

# Set EXTRA_CLING_ARGS="-I /usr/include" or wherever arbxx/cppyy.hpp can
# be resolved if the following line fails to find the header file.
cppyy.add_include_path(os.path.join(os.path.dirname(__file__), 'include'))

cppyy.include("arbxx/cppyy.hpp")
