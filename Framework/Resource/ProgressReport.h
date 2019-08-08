#pragma once
#include "Framework.h"

struct Progress final
{
	Progress()
	{
		Clear();
	}

	void Clear()
	{
		status.clear();
		jobs_done = 0;
		job_count = 0;
		is_Loading = false;
	}

	std::string status;
	int jobs_done;
	int job_count;
	bool is_Loading;
};

class ProgressReport final
{
public:
	static constexpr int Scene = 0;
	static constexpr int Model = 1;

public:
	static ProgressReport& Get()
	{
		static ProgressReport instance;
		return instance;
	}

	auto GetStatus(const int& progressID) -> const std::string
	{
		return reports[progressID].status;
	}

	void SetStatus(const int& progressID, const std::string& status)
	{
		reports[progressID].status = status;
	}

	auto IsLoading(const int& progressID) -> const bool
	{
		return reports[progressID].is_Loading;
	}

	void SetIsLoading(const int& progressID, const bool& is_Loading)
	{
		reports[progressID].is_Loading = is_Loading;
	}

	auto GetPercentage(const int& progressID) -> float
	{
		float jobs_done = static_cast<float>(reports[progressID].jobs_done);
		float job_count = static_cast<float>(reports[progressID].job_count);

		return jobs_done / job_count;
	}

	void SetJobsDone(const int& progressID, const int& jobs_done)
	{
		auto& report = reports[progressID];

		report.jobs_done = jobs_done;

		if (report.jobs_done >= report.job_count)
			report.is_Loading = false;
	}

	void SetJobCount(const int& progressID, const int& job_count)
	{
		auto& report = reports[progressID];

		report.job_count = job_count;
		report.is_Loading = true;
	}

	void IncrementJobsDone(const int& progressID)
	{
		auto& report = reports[progressID];

		report.jobs_done++;

		if (report.jobs_done >= report.job_count)
			report.is_Loading = false;
	}

	void Reset(const int& progressID)
	{
		reports[progressID].Clear();
	}

private:
	std::map<int, Progress> reports;

private:
	ProgressReport()
	{
		reports[Scene] = Progress();
		reports[Model] = Progress();
	}
	~ProgressReport() = default;
};