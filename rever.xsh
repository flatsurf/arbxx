######################################################################
#  This file is part of arbxx.
#
#        Copyright (C) 2022 Julian Rüth
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

import sys

try:
  input("Are you sure you are on the master branch which is identical to origin/master and the only pending changes are a version bump in the configure.ac of the library? [ENTER]")
except KeyboardInterrupt:
  sys.exit(1)

sys.path.insert(0, 'tools/rever')

import autodist

$PROJECT = 'arbxx'

$ACTIVITIES = [
    'version_bump',
    'changelog',
    'autodist',
    'tag',
    'push_tag',
    'ghrelease',
]

$VERSION_BUMP_PATTERNS = [
    ('configure.ac', r'AC_INIT', r'AC_INIT([arbxx], [$VERSION], [julian.rueth@fsfe.org])'),
    ('libarbxx/configure.ac', r'AC_INIT', r'AC_INIT([libarbxx], [$VERSION], [julian.rueth@fsfe.org])'),
    ('pyarbxx/configure.ac', r'AC_INIT', r'AC_INIT([pyarbxx], [$VERSION], [julian.rueth@fsfe.org])'),
    ('libarbxx/recipe/meta.yaml', r"\{% set version =", r"{% set version = '$VERSION' %}"),
    ('libarbxx/recipe/meta.yaml', r"\{% set build_number =", r"{% set build_number = '0' %}"),
    ('pyarbxx/recipe/meta.yaml', r"\{% set version =", r"{% set version = '$VERSION' %}"),
    ('pyarbxx/recipe/meta.yaml', r"\{% set build_number =", r"{% set build_number = '0' %}"),
]

$CHANGELOG_FILENAME = 'ChangeLog'
$CHANGELOG_TEMPLATE = 'TEMPLATE.rst'
$CHANGELOG_NEWS = 'doc/news'
$CHANGELOG_CATEGORIES = ('Added', 'Changed', 'Deprecated', 'Removed', 'Fixed', 'Performance')
$PUSH_TAG_REMOTE = 'git@github.com:flatsurf/arbxx.git'

$GITHUB_ORG = 'flatsurf'
$GITHUB_REPO = 'arbxx'

$GHRELEASE_ASSETS = ['arbxx-' + $VERSION + '.tar.gz']
