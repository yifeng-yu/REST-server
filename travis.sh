#!/bin/bash
curl -O http://chalonverse.com/435/pa7.tar.gz
tar xzf pa7.tar.gz
echo "Downloading IMDB data..."
curl -O https://datasets.imdbws.com/title.basics.tsv.gz
gunzip title.basics.tsv.gz

# Cmake into build directory
echo "Compiling..."
mkdir build
cd build
RELEASE=ON CC=clang CXX=clang++ cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON .. || exit 1
build_failed=0
make 2> >(tee diagnostics.txt >&2)|| { echo "::error::Code did not compile!"; build_failed=1; }
cd ..
./diagnostics-json.py
if [[ "$build_failed" == 1 ]] ; then
	exit 1
fi

cd build
# Run clang-tidy
echo "Running clang-tidy..."
../run-clang-tidy.py -quiet -header-filter=".*/src/[a-zA-Z].*" -export-fixes=tidy.yaml || { echo "::warning::Unable to run clang-tidy"; }
# Return to root folder (so cwd is correct)
cd ..
./tidy-json.py

# Try to spin up server
build/main title.basics.tsv &

# Try to connect to server
echo "Waiting for server to start up..."
wget --tries=7 --waitretry=2 --retry-connrefused http://localhost:12345/movie/id/tt0092099 || { echo "::error::Server took too long to start!"; exit 1; }

# Run graded tests
echo "Running tests..."
timeout 30 newman run postman/pa7-tests.postman_collection.json --color on || { echo "::error::Didn't pass all tests!"; exit 1; }
