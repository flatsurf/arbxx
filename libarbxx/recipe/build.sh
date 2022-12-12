export CXXFLAGS="$CXXFLAGS -g3"

cd libarbxx

autoreconf -ivf

./configure --prefix="$PREFIX" --without-benchmark --with-version-script --disable-static
make install
