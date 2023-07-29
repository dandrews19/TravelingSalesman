#!/bin/bash
# Cmake into build directory
curl -L -O http://chalonverse.com/435/pa4.tar.gz || { echo "::error::Unable to download graded tests. Try again."; exit 1; }
tar xzf pa4.tar.gz || { echo "::error::Error downloading graded tests. Try again."; exit 1; }
echo "Compiling..."
mkdir build
cd build
RELEASE=ON CC=gcc CXX=g++-11 cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON .. || exit 1
build_failed=0
make 2> >(tee diagnostics.txt >&2)
if [ "${PIPESTATUS[0]}" -ne "0" ] ; then
    echo "::error::Code did not compile!"
    echo -e "## \xF0\x9F\x9A\xA8\xF0\x9F\x9A\xA8 Code did not compile!! Your grade is currently a 0!! \xF0\x9F\x98\xAD\xF0\x9F\x98\xAD" >> ${GITHUB_STEP_SUMMARY}
	echo "### Build Log"  >> ${GITHUB_STEP_SUMMARY}
    echo -n "<pre>" >> ${GITHUB_STEP_SUMMARY}
    cat diagnostics.txt >> ${GITHUB_STEP_SUMMARY}
    echo "</pre>" >> ${GITHUB_STEP_SUMMARY}
	build_failed=1
fi

cd ..
./diagnostics-json.py
if [[ "$build_failed" == 1 ]] ; then
	exit 1
fi

cd build
# Run clang-tidy
echo "Running clang-tidy..."
../run-clang-tidy.py -quiet -header-filter=.*/src/.* -export-fixes=tidy.yaml | tee clang-tidy.txt 
if [ "${PIPESTATUS[0]}" -ne "0" ] ; then
	echo "::warn::clang-tidy failed to run"
fi
# Return to root folder (so cwd is correct)
cd ..
./tidy-json.py

# Run student tests
echo "Running student tests..."
timeout 30 build/tests/tests [student]
# Run graded tests
tests_failed=0
echo "Running graded tests..."
timeout 30 build/tests/tests [graded] -r=github
if [ "${PIPESTATUS[0]}" -ne "0" ] ; then
	tests_failed=1
fi

cd build
echo -e "\n## Compiler Warnings" >> ${GITHUB_STEP_SUMMARY}
if grep -q warning diagnostics.txt; then
	echo -e "\xE2\x9A\xA0 There are compiler warnings\n" >> ${GITHUB_STEP_SUMMARY}
	echo -en "<details closed><summary>Build Log</summary><pre>" >> ${GITHUB_STEP_SUMMARY}
	cat diagnostics.txt >> ${GITHUB_STEP_SUMMARY}
	echo -e "</pre></details>\n" >> ${GITHUB_STEP_SUMMARY}
else
	echo -e "\xE2\x9C\x85 There were no compiler warnings\n" >> ${GITHUB_STEP_SUMMARY}
fi

echo -e "\n## clang-tidy Warnings" >> ${GITHUB_STEP_SUMMARY}
if grep -q "warning:" clang-tidy.txt; then
	echo -e "\xE2\x9A\xA0 There are clang-tidy warnings\n" >> ${GITHUB_STEP_SUMMARY}
	echo -en "<details closed><summary>clang-tidy Log</summary><pre>" >> ${GITHUB_STEP_SUMMARY}
	cat clang-tidy.txt >> ${GITHUB_STEP_SUMMARY}
	echo -e "</pre></details>\n" >> ${GITHUB_STEP_SUMMARY}
else
	echo -e "\xE2\x9C\x85 There were no clang-tidy warnings\n" >> ${GITHUB_STEP_SUMMARY}
fi

if [[ "$tests_failed" == 1 ]] ; then
	echo "::error::Not all graded tests passed!"
	exit 1
fi
