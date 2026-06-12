#include "TaskManager.hpp"
#include "imgui.h"
#include <algorithm>
#include <cstdlib>

TaskManager::TaskManager() {
	// Dummy processes
	processes = {
		{ 1024, "explorer.exe", "Running", 2.3f, 145.2f },
		{ 1024, "csopesy.exe", "Running", 12.7f, 210.5f },
		{ 1024, "chrome.exe", "Running", 18.4f, 890.3f },
		{ 1024, "vscode.exe", "Running", 1.2f, 410.9f },
		{ 1024, "discord.exe", "Running", 4.1f, 320.7f },
	};
}

void TaskManager::drawPerformanceTab() {
	ImGui::Text("CPU Usage: %.0f%%", currentCpuUsage);
	ImGui::ProgressBar(currentCpuUsage / 100.0f, ImVec2(-1.0f, 0), "");
	
	ImGui::Text("Memory Usage: %.0f%%", currentMemUsage);
	ImGui::ProgressBar(currentMemUsage / 100.0f, ImVec2(-1.0f, 0), "");
}

void TaskManager::drawProcessesTab() {
	if (ImGui::BeginTable("ProcessTable", 5,
				ImGuiTableFlags_Sortable |
				ImGuiTableFlags_Resizable |
				ImGuiTableFlags_Borders |
				ImGuiTableFlags_RowBg)) {
		ImGui::TableSetupColumn("PID", ImGuiTableColumnFlags_DefaultSort);
		ImGui::TableSetupColumn("Name");
		ImGui::TableSetupColumn("State");
		ImGui::TableSetupColumn("CPU #");
		ImGui::TableSetupColumn("Memory");
		ImGui::TableHeadersRow();

		// Create processes
		for (const auto& process : processes) {
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("%d", process.pid);
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%s", process.name.c_str());
			ImGui::TableSetColumnIndex(2);
			ImGui::Text("%s", process.state.c_str());
			ImGui::TableSetColumnIndex(3);
			ImGui::Text("%.1f%%", process.cpuUsage);
			ImGui::TableSetColumnIndex(4);
			ImGui::Text("%.1f MB", process.memoryUsage);
		}

		ImGui::EndTable();
	}
}