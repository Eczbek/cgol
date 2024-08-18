#include <chrono>
#include <cstddef>
#include <cstdio>
#include <print>
#include <random>
#include <thread>
#include <vector>

#include "../xieite/include/xieite/streams/color.hpp"
#include "../xieite/include/xieite/streams/position.hpp"
#include "../xieite/include/xieite/streams/standard_handle.hpp"

int main() {
	std::vector<std::vector<bool>> world;

	std::mt19937 rng = std::mt19937(std::random_device()());
	std::bernoulli_distribution dist;

	auto terminal = xieite::streams::StandardHandle(stdin, stdout);
	terminal.setInputEcho(false);
	terminal.setInputSignals(false);
	terminal.setScreenAlternate(true);
	terminal.setCursorAlternate(true);
	terminal.setCursorInvisible(true);
	terminal.clearScreen();

	std::vector<std::vector<xieite::streams::Color<3>>> previousFrame;

	bool running = true;
	for (std::size_t tick = 0; running; ++tick) {
		const xieite::streams::Position size = terminal.getScreenSize();
		const std::size_t frameHeight = static_cast<std::size_t>(size.row) * 2;
		const std::size_t frameWidth = static_cast<std::size_t>(size.column);

		const std::size_t worldHeight = world.size();
		world.resize(frameHeight);
		for (auto& row : world) {
			const std::size_t worldWidth = row.size();
			row.resize(frameWidth);
			for (std::size_t j = worldWidth; j < frameWidth; ++j) {
				row[j] = dist(rng);
			}
		}

		std::vector<std::vector<xieite::streams::Color<3>>> currentFrame;
		currentFrame.resize(frameHeight);
		for (auto& row : currentFrame) {
			row.resize(frameWidth);
		}
		for (std::size_t i = 0; i < frameHeight; ++i) {
			const std::size_t u = (i + frameHeight - 1) % frameHeight;
			const std::size_t d = (i + 1) % frameHeight;
			for (std::size_t j = 0; j < frameWidth; ++j) {
				const std::size_t l = (j + frameWidth - 1) % frameWidth;
				const std::size_t r = (j + 1) % frameWidth;
				const std::size_t n = world[u][l] + world[u][j] + world[u][r] + world[i][l] + world[i][r] + world[d][l] + world[d][j] + world[d][r];
				currentFrame[i][j] = xieite::streams::Color<3>(0xFFFFFF * (n > 1) * (n < 4) * (world[i][j] || (n == 3)));
			}
		}
		for (std::size_t i = 0; i < frameHeight; ++i) {
			for (std::size_t j = 0; j < frameWidth; ++j) {
				world[i][j] = !!currentFrame[i][j].value();
			}
		}

		if (currentFrame != previousFrame) {
			previousFrame.resize(frameHeight);
			for (auto& row : previousFrame) {
				row.resize(frameWidth);
			}
			std::string display;
			xieite::streams::Position cursor;
			for (std::size_t y = 0; y < frameHeight; y += 2) {
				for (std::size_t x = 0; x < frameWidth; ++x) {
					if ((currentFrame[y][x] == previousFrame[y][x]) && (currentFrame[y + 1][x] == previousFrame[y + 1][x])) {
						continue;
					}
					display += terminal.stringSetCursorPosition(xieite::streams::Position(static_cast<int>(y / 2), static_cast<int>(x)));
					display += terminal.stringSetForegroundColor(currentFrame[y][x]);
					display += terminal.stringSetBackgroundColor(currentFrame[y + 1][x]);
					display += "▀";
					display += terminal.stringResetStyles();
				}
			}
			previousFrame = currentFrame;
			std::print(terminal.outputFile, "{}", display);
			std::fflush(terminal.outputFile);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		const std::string input = terminal.readString();
		switch (input[input.size() - 1]) {
		case 'Q':
		case 'q':
			running = false;
			break;
		}
	}
}
