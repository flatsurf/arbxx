export CXXFLAGS="$CXXFLAGS -g3"

cd pyarbxx

autoreconf -ivf

./configure --prefix="$PREFIX" --without-benchmark --without-pytest
make install
