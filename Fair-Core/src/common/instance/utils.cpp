#include "..\utils.h"
#include <sstream>
#include <fstream>
void utils::writeLog(const std::string& message) {
	 //指定日志文件路径，这里是 C 盘的某个文件夹
	std::string filePath = "C:\\Leave\\log22.txt";

	// 打开文件以追加方式写入
	std::ofstream logFile(filePath, std::ios::app);

	// 检查文件是否成功打开
	if (logFile.is_open()) {
		// 获取当前时间
		std::time_t now = std::time(nullptr);
		std::tm* timeInfo = std::localtime(&now);

		// 格式化时间
		char timeBuffer[80];
		std::strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", timeInfo);

		// 将时间和日志信息写入文件
		logFile << timeBuffer << " - " << message << std::endl;
	}
	else {
		std::cerr << "无法打开日志文件！" << std::endl;
	}
}