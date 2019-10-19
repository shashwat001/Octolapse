////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Octolapse - A plugin for OctoPrint used for making stabilized timelapse videos.
// Copyright(C) 2019  Brad Hochgesang
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This program is free software : you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.If not, see the following :
// https ://github.com/FormerLurker/Octolapse/blob/master/LICENSE
//
// You can contact the author either through the git - hub repository, or at the
// following email address : FormerLurker@pm.me
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stabilization_results.h"
#include "logging.h"
stabilization_results::stabilization_results()
{
	bool success = false;
	double seconds_elapsed = 0;
	long gcodes_processed = 0;
	long lines_processed = 0;
}

PyObject* stabilization_results::to_py_object()
{
	PyObject * py_snapshot_plans = snapshot_plan::build_py_object(snapshot_plans);
	if (py_snapshot_plans == NULL)
	{
		return NULL;
	}

	// Create stabilization quality issues list
	PyObject *py_quality_issues = PyList_New(0);
	if (py_quality_issues == NULL)
	{
		std::string message = "stabilization_results.to_py_object - Unable to create py_issues PyList object.";
		octolapse_log_exception(octolapse_log::SNAPSHOT_PLAN, message);
		return NULL;
	}

	// Create each quality issue and append to list
	for (unsigned int issue_index = 0; issue_index < quality_issues.size(); issue_index++)
	{
		PyObject * py_issue = quality_issues[issue_index].to_py_object();
		if (py_issue == NULL)
		{
			return NULL;
		}
		bool success = !(PyList_Append(py_quality_issues, py_issue) < 0); // reference to pSnapshotPlan stolen
		if (!success)
		{
			std::string message = "stabilization_results.to_py_object - Unable to append the quality issue to the quality issues list.";
			octolapse_log_exception(octolapse_log::SNAPSHOT_PLAN, message);
			return NULL;
		}
		// Need to decref after PyList_Append, since it increfs the PyObject
		Py_DECREF(py_issue);
	}

	// Create each processing issue and append to list
	PyObject *py_processing_issues = PyList_New(0);
	for (unsigned int issue_index = 0; issue_index < processing_issues.size(); issue_index++)
	{
		PyObject * py_issue = processing_issues[issue_index].to_py_object();
		if (py_issue == NULL)
		{
			return NULL;
		}
		bool success = !(PyList_Append(py_processing_issues, py_issue) < 0); // reference to pSnapshotPlan stolen
		if (!success)
		{
			std::string message = "stabilization_results.to_py_object - Unable to append the processing issue to the quality issues list.";
			octolapse_log_exception(octolapse_log::SNAPSHOT_PLAN, message);
			return NULL;
		}
		// Need to decref after PyList_Append, since it increfs the PyObject
		Py_DECREF(py_issue);
	}
	
	PyObject * py_results = Py_BuildValue("(l,s,O,d,l,l,l,O,O)", success, errors.c_str(), py_snapshot_plans, seconds_elapsed, gcodes_processed, lines_processed, missed_layer_count, py_quality_issues, py_processing_issues);
	if (py_results == NULL)
	{
		std::string message = "stabilization_results.to_py_object - Unable to create a Tuple from the snapshot plan list.";
		octolapse_log_exception(octolapse_log::SNAPSHOT_PLAN, message);
		return NULL;
	}
	Py_DECREF(py_snapshot_plans);
	Py_DECREF(py_quality_issues);
	Py_DECREF(py_processing_issues);

	return py_results;
}


PyObject * stabilization_quality_issue::to_py_object() const
{
	PyObject * py_results = Py_BuildValue("(l,s)", static_cast<int>(issue_type), description.c_str() );
	if (py_results == NULL)
	{
		std::string message = "stabilization_quality_issue.to_py_object - Unable to create a Tuple from a stabilization_quality_issue.";
		octolapse_log_exception(octolapse_log::SNAPSHOT_PLAN, message);
		return NULL;
	}
	return py_results;
}

PyObject * stabilization_processing_issue::to_py_object() const
{
	PyObject * py_results = Py_BuildValue("(l,s)", static_cast<int>(issue_type), description.c_str());
	if (py_results == NULL)
	{
		std::string message = "stabilization_processing_issue.to_py_object - Unable to create a Tuple from a stabilization_processing_issue.";
		octolapse_log_exception(octolapse_log::SNAPSHOT_PLAN, message);
		return NULL;
	}
	return py_results;
}
