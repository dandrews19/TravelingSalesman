#ifndef CATCH_REPORTER_GITHUB_HPP_INCLUDED
#define CATCH_REPORTER_GITHUB_HPP_INCLUDED

#include <cstring>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>

#ifdef __clang__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wpadded"
#endif

static inline std::vector<std::string> split(const std::string& str, char delim = '|') {
	std::vector<std::string> retVal;

	size_t start = 0;
	size_t delimLoc = str.find_first_of(delim, start);
	while (delimLoc != std::string::npos)
	{
		retVal.emplace_back(str.substr(start, delimLoc - start));

		start = delimLoc + 1;
		delimLoc = str.find_first_of(delim, start);
	}

	retVal.emplace_back(str.substr(start));
	return retVal;
}

namespace Catch {

	struct GitHubReporter : StreamingReporterBase<GitHubReporter> {
		std::unique_ptr<TablePrinter> m_tablePrinter;

		GitHubReporter(ReporterConfig const& config);
		~GitHubReporter() override;
		static std::string getDescription();

		void noMatchingTestCases(std::string const& spec) override;

		void reportInvalidArguments(std::string const& arg) override;

		void assertionStarting(AssertionInfo const&) override;

		bool assertionEnded(AssertionStats const& _assertionStats) override;

		void sectionStarting(SectionInfo const& _sectionInfo) override;
		void sectionEnded(SectionStats const& _sectionStats) override;

#if defined(CATCH_CONFIG_ENABLE_BENCHMARKING)
		void benchmarkPreparing(std::string const& name) override;
		void benchmarkStarting(BenchmarkInfo const& info) override;
		void benchmarkEnded(BenchmarkStats<> const& stats) override;
		void benchmarkFailed(std::string const& error) override;
#endif // CATCH_CONFIG_ENABLE_BENCHMARKING

		void testCaseStarting(TestCaseInfo const& _testInfo) override;
		void testCaseEnded(TestCaseStats const& _testCaseStats) override;
		void testGroupEnded(TestGroupStats const& _testGroupStats) override;
		void testRunEnded(TestRunStats const& _testRunStats) override;
		void testRunStarting(TestRunInfo const& _testRunInfo) override;
	private:

		void lazyPrint();

		void lazyPrintWithoutClosingBenchmarkTable();
		void lazyPrintRunInfo();
		void lazyPrintGroupInfo();
		void printTestCaseAndSectionHeader();

		void printClosedHeader(std::string const& _name);
		void printOpenHeader(std::string const& _name);

		// if string has a : in first line will set indent to follow it on
		// subsequent lines
		void printHeaderString(std::string const& _string, std::size_t indent = 0);

		void printTotals(Totals const& totals);
		void printSummaryRow(std::string const& label, std::vector<SummaryColumn> const& cols, std::size_t row);

		void printTotalsDivider(Totals const& totals);
		void printSummaryDivider();
		void printTestFilters();

	private:
		std::ofstream m_markdownFile;
		bool m_headerPrinted = false;
		int m_subTotal = 0;
		int m_maxSubTotal = 0;
		int m_totalScore = 0;
		int m_maxScore = 0;
		std::string m_lastAssertMsg;
	};

	namespace {

		// Formatter impl for GitHubReporter
		class GitHubAssertionPrinter {
		public:
			GitHubAssertionPrinter& operator= (GitHubAssertionPrinter const&) = delete;
			GitHubAssertionPrinter(GitHubAssertionPrinter const&) = delete;
			GitHubAssertionPrinter(std::ostream& _stream, AssertionStats const& _stats, bool _printInfoMessages)
				: stream(_stream),
				stats(_stats),
				result(_stats.assertionResult),
				colour(Colour::None),
				message(result.getMessage()),
				messages(_stats.infoMessages),
				printInfoMessages(_printInfoMessages) {
				switch (result.getResultType()) {
				case ResultWas::Ok:
					colour = Colour::Success;
					passOrFail = "PASSED";
					//if( result.hasMessage() )
					if (_stats.infoMessages.size() == 1)
						messageLabel = "with message";
					if (_stats.infoMessages.size() > 1)
						messageLabel = "with messages";
					break;
				case ResultWas::ExpressionFailed:
					if (result.isOk()) {
						colour = Colour::Success;
						passOrFail = "FAILED - but was ok";
					}
					else {
						colour = Colour::Error;
						passOrFail = "FAILED";
					}
					if (_stats.infoMessages.size() == 1)
						messageLabel = "with message";
					if (_stats.infoMessages.size() > 1)
						messageLabel = "with messages";
					break;
				case ResultWas::ThrewException:
					colour = Colour::Error;
					passOrFail = "FAILED";
					messageLabel = "due to unexpected exception with ";
					if (_stats.infoMessages.size() == 1)
						messageLabel += "message";
					if (_stats.infoMessages.size() > 1)
						messageLabel += "messages";
					break;
				case ResultWas::FatalErrorCondition:
					colour = Colour::Error;
					passOrFail = "FAILED";
					messageLabel = "due to a fatal error condition";
					break;
				case ResultWas::DidntThrowException:
					colour = Colour::Error;
					passOrFail = "FAILED";
					messageLabel = "because no exception was thrown where one was expected";
					break;
				case ResultWas::Info:
					messageLabel = "info";
					break;
				case ResultWas::Warning:
					messageLabel = "warning";
					break;
				case ResultWas::ExplicitFailure:
					passOrFail = "FAILED";
					colour = Colour::Error;
					if (_stats.infoMessages.size() == 1)
						messageLabel = "explicitly with message";
					if (_stats.infoMessages.size() > 1)
						messageLabel = "explicitly with messages";
					break;
					// These cases are here to prevent compiler warnings
				case ResultWas::Unknown:
				case ResultWas::FailureBit:
				case ResultWas::Exception:
					passOrFail = "** internal error **";
					colour = Colour::Error;
					break;
				}
			}

			void print() const {
				printSourceInfo();
				if (stats.totals.assertions.total() > 0) {
					printResultType();
					printOriginalExpression();
					printReconstructedExpression();
				}
				else {
					stream << '\n';
				}
				printMessage();
			}

		private:
			void printResultType() const {
				if (!passOrFail.empty()) {
					Colour colourGuard(colour);
					stream << passOrFail << ":\n";
				}
			}
			void printOriginalExpression() const {
				if (result.hasExpression()) {
					Colour colourGuard(Colour::OriginalExpression);
					stream << "  ";
					stream << result.getExpressionInMacro();
					stream << '\n';
				}
			}
			void printReconstructedExpression() const {
				if (result.hasExpandedExpression()) {
					stream << "with expansion:\n";
					Colour colourGuard(Colour::ReconstructedExpression);
					stream << Column(result.getExpandedExpression()).indent(2) << '\n';
				}
			}
			void printMessage() const {
				if (!messageLabel.empty())
					stream << messageLabel << ':' << '\n';
				for (auto const& msg : messages) {
					// If this assertion is a warning ignore any INFO messages
					if (printInfoMessages || msg.type != ResultWas::Info) {
						int indent = 2;
						if (msg.type == ResultWas::ExplicitFailure) {
							stream << "error: ";
							indent = 0;
						}
						else if (msg.type == ResultWas::Warning) {
							stream << "warning: ";
							indent = 0;
						}
						stream << Column(msg.message).indent(indent) << '\n';
					}
				}
			}
			void printSourceInfo() const {
				Colour colourGuard(Colour::FileName);
				stream << result.getSourceInfo() << ": ";
			}

			std::ostream& stream;
			AssertionStats const& stats;
			AssertionResult const& result;
			Colour::Code colour;
			std::string passOrFail;
			std::string messageLabel;
			std::string message;
			std::vector<MessageInfo> messages;
			bool printInfoMessages;
		};
	} // end anon namespace

	GitHubReporter::GitHubReporter(ReporterConfig const& config)
		: StreamingReporterBase(config),
		m_tablePrinter(new TablePrinter(config.stream(),
			[&config]() -> std::vector<ColumnInfo> {
				if (config.fullConfig()->benchmarkNoAnalysis())
				{
					return{
						{ "benchmark name", CATCH_CONFIG_CONSOLE_WIDTH - 43, ColumnInfo::Left },
						{ "     samples", 14, ColumnInfo::Right },
						{ "  iterations", 14, ColumnInfo::Right },
						{ "        mean", 14, ColumnInfo::Right }
					};
				}
				else
				{
					return{
						{ "benchmark name", CATCH_CONFIG_CONSOLE_WIDTH - 43, ColumnInfo::Left },
						{ "samples      mean       std dev", 14, ColumnInfo::Right },
						{ "iterations   low mean   low std dev", 14, ColumnInfo::Right },
						{ "estimated    high mean  high std dev", 14, ColumnInfo::Right }
					};
				}
			}())) {
		const char* githubFileName = std::getenv("GITHUB_STEP_SUMMARY");
		if (githubFileName) {
			m_markdownFile.open(githubFileName);
		}
		else {
			m_markdownFile.open("GradeReport.md");
		}
		m_markdownFile << "## Grade Report\n";
		m_markdownFile << "<table>\n";
		m_markdownFile << "<thead><tr><td><b>Result</b></td><td><b>Test</b></td>"
			"<td><b>Points</b></td><td><b>Earned</b></td><td><b>Details</b></td></tr></thead>\n";
		m_markdownFile.flush();
	}
	GitHubReporter::~GitHubReporter() {
		m_markdownFile << "<tr><td colspan='2'><b>TOTAL</b></td>";
		m_markdownFile << "<td><b>" << m_maxScore << "</b></td>";
		m_markdownFile << "<td><b>" << m_totalScore << "</b></th><td></td></tr>";
		m_markdownFile << "</table>\n";
		if (m_maxScore == m_totalScore) {
			m_markdownFile << "\n## \xF0\x9F\x94\xA5 All test cases passed!! \xF0\x9F\x98\x80\n\n";
			m_markdownFile << "You received all " << m_maxScore << " points.\n\n";
		}
		else {
			m_markdownFile << "\n## \xF0\x9F\x9A\xA8\xF0\x9F\x9A\xA8 Some test cases failed!! \xF0\x9F\x98\xAD\xF0\x9F\x98\xAD\n\n";
			m_markdownFile << "You received " << m_totalScore << " out of " << m_maxScore << " points.\n\n";
		}
		m_markdownFile << "Keep in mind you still need to check for warnings, and there may "
			"be additional assignment-specific points." << std::endl;
	}

	std::string GitHubReporter::getDescription() {
		return "Like console reporter but for GitHub Actions outputs markdown also";
	}

	void GitHubReporter::noMatchingTestCases(std::string const& spec) {
		stream << "No test cases matched '" << spec << '\'' << std::endl;
	}

	void GitHubReporter::reportInvalidArguments(std::string const& arg) {
		stream << "Invalid Filter: " << arg << std::endl;
	}

	void GitHubReporter::assertionStarting(AssertionInfo const&) {}

	bool GitHubReporter::assertionEnded(AssertionStats const& _assertionStats) {
		AssertionResult const& result = _assertionStats.assertionResult;

		bool includeResults = m_config->includeSuccessfulResults() || !result.isOk();

		// Drop out if result was successful but we're not printing them.
		if (!includeResults && result.getResultType() != ResultWas::Warning)
			return false;

		lazyPrint();

		GitHubAssertionPrinter printer(stream, _assertionStats, includeResults);
		printer.print();
		stream << std::endl;

		if (result.isOk()) {
			m_lastAssertMsg.clear();
		}
		else {
			std::ostringstream sstream;
			GitHubAssertionPrinter stringPrinter(sstream, _assertionStats, includeResults);
			stringPrinter.print();
			m_lastAssertMsg = sstream.str();
		}

		return true;
	}

	void GitHubReporter::sectionStarting(SectionInfo const& _sectionInfo) {
		m_tablePrinter->close();
		m_headerPrinted = false;
		m_lastAssertMsg.clear();
		StreamingReporterBase::sectionStarting(_sectionInfo);
	}
	void GitHubReporter::sectionEnded(SectionStats const& _sectionStats) {
		if (_sectionStats.sectionInfo.name != currentTestCaseInfo->name) {
			std::vector<std::string> splits = split(_sectionStats.sectionInfo.name);
			int points = 0;
			if (splits.size() == 2) {
				try { points = std::stoi(splits[1]); }
				catch (...) {}
			}
			m_maxSubTotal += points;

			std::string result;
			int earned = 0;
			if (_sectionStats.assertions.failed > 0) {
				result = "\xE2\x9D\x8C FAIL";
			}
			else {
				result = "\xE2\x9C\x85 PASS";
				earned = points;
			}
			m_subTotal += earned;

			m_markdownFile << "<tr><td>" << result << "</td>\n";
			m_markdownFile << "<td>" << splits[0] << "</td>\n";
			m_markdownFile << "<td>" << points << "</td>\n";
			m_markdownFile << "<td>" << earned << "</td>\n";
			if (!m_lastAssertMsg.empty()) {
				m_markdownFile << "<td><details closed><summary>Failure Info</summary>\n";
				m_markdownFile << "<pre>\n" << m_lastAssertMsg << "</pre></details></td>\n";
			}
			else {
				m_markdownFile << "<td></td>\n";
			}
			m_markdownFile << "</tr>" << std::endl;
		}

		m_tablePrinter->close();
		if (_sectionStats.assertions.failed > 0 && _sectionStats.sectionInfo.name != currentTestCaseInfo->name) {
			stream << "Test case failed: " << _sectionStats.sectionInfo.name << "'\n";
		}
		if (_sectionStats.missingAssertions) {
			lazyPrint();
			Colour colour(Colour::ResultError);
			if (m_sectionStack.size() > 1)
				stream << "\nNo assertions in section";
			else
				stream << "\nNo assertions in test case";
			stream << " '" << _sectionStats.sectionInfo.name << "'\n" << std::endl;
		}
		double dur = _sectionStats.durationInSeconds;
		if (shouldShowDuration(*m_config, dur)) {
			stream << getFormattedDuration(dur) << " s: " << _sectionStats.sectionInfo.name << std::endl;
		}
		if (m_headerPrinted) {
			m_headerPrinted = false;
		}
		StreamingReporterBase::sectionEnded(_sectionStats);
	}

#if defined(CATCH_CONFIG_ENABLE_BENCHMARKING)
	void GitHubReporter::benchmarkPreparing(std::string const& name) {
		lazyPrintWithoutClosingBenchmarkTable();

		auto nameCol = Column(name).width(static_cast<std::size_t>(m_tablePrinter->columnInfos()[0].width - 2));

		bool firstLine = true;
		for (auto line : nameCol) {
			if (!firstLine)
				(*m_tablePrinter) << ColumnBreak() << ColumnBreak() << ColumnBreak();
			else
				firstLine = false;

			(*m_tablePrinter) << line << ColumnBreak();
		}
	}

	void GitHubReporter::benchmarkStarting(BenchmarkInfo const& info) {
		(*m_tablePrinter) << info.samples << ColumnBreak()
			<< info.iterations << ColumnBreak();
		if (!m_config->benchmarkNoAnalysis())
			(*m_tablePrinter) << Duration(info.estimatedDuration) << ColumnBreak();
	}
	void GitHubReporter::benchmarkEnded(BenchmarkStats<> const& stats) {
		if (m_config->benchmarkNoAnalysis())
		{
			(*m_tablePrinter) << Duration(stats.mean.point.count()) << ColumnBreak();
		}
		else
		{
			(*m_tablePrinter) << ColumnBreak()
				<< Duration(stats.mean.point.count()) << ColumnBreak()
				<< Duration(stats.mean.lower_bound.count()) << ColumnBreak()
				<< Duration(stats.mean.upper_bound.count()) << ColumnBreak() << ColumnBreak()
				<< Duration(stats.standardDeviation.point.count()) << ColumnBreak()
				<< Duration(stats.standardDeviation.lower_bound.count()) << ColumnBreak()
				<< Duration(stats.standardDeviation.upper_bound.count()) << ColumnBreak() << ColumnBreak() << ColumnBreak() << ColumnBreak() << ColumnBreak();
		}
	}

	void GitHubReporter::benchmarkFailed(std::string const& error) {
		Colour colour(Colour::Red);
		(*m_tablePrinter)
			<< "Benchmark failed (" << error << ')'
			<< ColumnBreak() << RowBreak();
	}
#endif // CATCH_CONFIG_ENABLE_BENCHMARKING

	void GitHubReporter::testCaseStarting(TestCaseInfo const& _testInfo) {
		StreamingReporterBase::testCaseStarting(_testInfo);
		m_markdownFile << "<tr><td colspan='5'><b>" << _testInfo.name << "</b></td></tr>" << std::endl;
		m_subTotal = 0;
		m_maxSubTotal = 0;
	}
	void GitHubReporter::testCaseEnded(TestCaseStats const& _testCaseStats) {
		m_tablePrinter->close();
		StreamingReporterBase::testCaseEnded(_testCaseStats);
		m_headerPrinted = false;
		m_markdownFile << "<tr><td></td><td><b>Subtotal</b></td><td>" << m_maxSubTotal << "</td>";
		m_markdownFile << "<td>" << m_subTotal << "</td>";
		m_markdownFile << "<td></td></tr>" << std::endl;

		m_totalScore += m_subTotal;
		m_maxScore += m_maxSubTotal;
		m_subTotal = 0;
		m_maxSubTotal = 0;
	}
	void GitHubReporter::testGroupEnded(TestGroupStats const& _testGroupStats) {
		if (currentGroupInfo.used) {
			printSummaryDivider();
			stream << "Summary for group '" << _testGroupStats.groupInfo.name << "':\n";
			printTotals(_testGroupStats.totals);
			stream << '\n' << std::endl;
		}
		StreamingReporterBase::testGroupEnded(_testGroupStats);
	}
	void GitHubReporter::testRunEnded(TestRunStats const& _testRunStats) {
		printTotalsDivider(_testRunStats.totals);
		printTotals(_testRunStats.totals);
		stream << std::endl;
		StreamingReporterBase::testRunEnded(_testRunStats);
	}
	void GitHubReporter::testRunStarting(TestRunInfo const& _testInfo) {
		StreamingReporterBase::testRunStarting(_testInfo);
		printTestFilters();
	}

	void GitHubReporter::lazyPrint() {

		m_tablePrinter->close();
		lazyPrintWithoutClosingBenchmarkTable();
	}

	void GitHubReporter::lazyPrintWithoutClosingBenchmarkTable() {

		if (!currentTestRunInfo.used)
			lazyPrintRunInfo();
		if (!currentGroupInfo.used)
			lazyPrintGroupInfo();

		if (!m_headerPrinted) {
			printTestCaseAndSectionHeader();
			m_headerPrinted = true;
		}
	}
	void GitHubReporter::lazyPrintRunInfo() {
		stream << '\n' << getLineOfChars<'~'>() << '\n';
		Colour colour(Colour::SecondaryText);
		stream << currentTestRunInfo->name
			<< " is a Catch v" << libraryVersion() << " host application.\n"
			<< "Run with -? for options\n\n";

		if (m_config->rngSeed() != 0)
			stream << "Randomness seeded to: " << m_config->rngSeed() << "\n\n";

		currentTestRunInfo.used = true;
	}
	void GitHubReporter::lazyPrintGroupInfo() {
		if (!currentGroupInfo->name.empty() && currentGroupInfo->groupsCounts > 1) {
			printClosedHeader("Group: " + currentGroupInfo->name);
			currentGroupInfo.used = true;
		}
	}
	void GitHubReporter::printTestCaseAndSectionHeader() {
		assert(!m_sectionStack.empty());
		printOpenHeader(currentTestCaseInfo->name);

		if (m_sectionStack.size() > 1) {
			Colour colourGuard(Colour::Headers);

			auto
				it = m_sectionStack.begin() + 1, // Skip first section (test case)
				itEnd = m_sectionStack.end();
			for (; it != itEnd; ++it)
				printHeaderString(it->name, 2);
		}

		SourceLineInfo lineInfo = m_sectionStack.back().lineInfo;

		stream << getLineOfChars<'-'>() << '\n';
		Colour colourGuard(Colour::FileName);
		stream << lineInfo << '\n';
		stream << getLineOfChars<'.'>() << '\n' << std::endl;
	}

	void GitHubReporter::printClosedHeader(std::string const& _name) {
		printOpenHeader(_name);
		stream << getLineOfChars<'.'>() << '\n';
	}
	void GitHubReporter::printOpenHeader(std::string const& _name) {
		stream << getLineOfChars<'-'>() << '\n';
		{
			Colour colourGuard(Colour::Headers);
			printHeaderString(_name);
		}
	}

	// if string has a : in first line will set indent to follow it on
	// subsequent lines
	void GitHubReporter::printHeaderString(std::string const& _string, std::size_t indent) {
		std::size_t i = _string.find(": ");
		if (i != std::string::npos)
			i += 2;
		else
			i = 0;
		stream << Column(_string).indent(indent + i).initialIndent(indent) << '\n';
	}


	void GitHubReporter::printTotals(Totals const& totals) {
		if (totals.testCases.total() == 0) {
			stream << Colour(Colour::Warning) << "No tests ran\n";
		}
		else if (totals.assertions.total() > 0 && totals.testCases.allPassed()) {
			stream << Colour(Colour::ResultSuccess) << "All tests passed";
			stream << " ("
				<< pluralise(totals.assertions.passed, "assertion") << " in "
				<< pluralise(totals.testCases.passed, "test case") << ')'
				<< '\n';
		}
		else {

			std::vector<SummaryColumn> columns;
			columns.push_back(SummaryColumn("", Colour::None)
				.addRow(totals.testCases.total())
				.addRow(totals.assertions.total()));
			columns.push_back(SummaryColumn("passed", Colour::Success)
				.addRow(totals.testCases.passed)
				.addRow(totals.assertions.passed));
			columns.push_back(SummaryColumn("failed", Colour::ResultError)
				.addRow(totals.testCases.failed)
				.addRow(totals.assertions.failed));
			columns.push_back(SummaryColumn("failed as expected", Colour::ResultExpectedFailure)
				.addRow(totals.testCases.failedButOk)
				.addRow(totals.assertions.failedButOk));

			printSummaryRow("test cases", columns, 0);
			printSummaryRow("assertions", columns, 1);
		}
	}

	void GitHubReporter::printSummaryRow(std::string const& label, std::vector<SummaryColumn> const& cols, std::size_t row) {
		for (auto col : cols) {
			std::string value = col.rows[row];
			if (col.label.empty()) {
				stream << label << ": ";
				if (value != "0")
					stream << value;
				else
					stream << Colour(Colour::Warning) << "- none -";
			}
			else if (value != "0") {
				stream << Colour(Colour::LightGrey) << " | ";
				stream << Colour(col.colour)
					<< value << ' ' << col.label;
			}
		}
		stream << '\n';
	}

	void GitHubReporter::printTotalsDivider(Totals const& totals) {
		if (totals.testCases.total() > 0) {
			std::size_t failedRatio = makeRatio(totals.testCases.failed, totals.testCases.total());
			std::size_t failedButOkRatio = makeRatio(totals.testCases.failedButOk, totals.testCases.total());
			std::size_t passedRatio = makeRatio(totals.testCases.passed, totals.testCases.total());
			while (failedRatio + failedButOkRatio + passedRatio < CATCH_CONFIG_CONSOLE_WIDTH - 1)
				findMax(failedRatio, failedButOkRatio, passedRatio)++;
			while (failedRatio + failedButOkRatio + passedRatio > CATCH_CONFIG_CONSOLE_WIDTH - 1)
				findMax(failedRatio, failedButOkRatio, passedRatio)--;

			stream << Colour(Colour::Error) << std::string(failedRatio, '=');
			stream << Colour(Colour::ResultExpectedFailure) << std::string(failedButOkRatio, '=');
			if (totals.testCases.allPassed())
				stream << Colour(Colour::ResultSuccess) << std::string(passedRatio, '=');
			else
				stream << Colour(Colour::Success) << std::string(passedRatio, '=');
		}
		else {
			stream << Colour(Colour::Warning) << std::string(CATCH_CONFIG_CONSOLE_WIDTH - 1, '=');
		}
		stream << '\n';
	}
	void GitHubReporter::printSummaryDivider() {
		stream << getLineOfChars<'-'>() << '\n';
	}

	void GitHubReporter::printTestFilters() {
		if (m_config->testSpec().hasFilters()) {
			Colour guard(Colour::BrightYellow);
			stream << "Filters: " << serializeFilters(m_config->getTestsOrTags()) << '\n';
		}
	}

	CATCH_REGISTER_REPORTER("github", GitHubReporter)

		}

#ifdef __clang__
#   pragma clang diagnostic pop
#endif

#endif
