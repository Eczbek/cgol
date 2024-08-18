#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
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

	// std::vector<std::vector<xieite::streams::Color<4>>> previousFrame;
	std::vector<std::vector<bool>> previousFrame;

	// std::size_t offsetX = 0;
	// std::size_t offsetY = 0;

	bool running = true;
	for (std::size_t tick = 0; running; ++tick) {
		const xieite::streams::Position size = terminal.getScreenSize();
		const std::size_t frameHeight = static_cast<std::size_t>(size.row) * 2;
		const std::size_t frameWidth = static_cast<std::size_t>(size.column);

		// std::vector<std::vector<xieite::streams::Color<4>>> currentFrame;
		// currentFrame.resize(frameHeight);
		// for (auto& row : currentFrame) {
		// 	row.resize(frameWidth);
		// }

		const std::size_t worldHeight = world.size();
		world.resize(frameHeight);
		for (auto& row : world) {
			const std::size_t worldWidth = row.size();
			row.resize(frameWidth);
			for (std::size_t j = worldWidth; j < frameWidth; ++j) {
				row[j] = dist(rng);
			}
		}

		std::vector<std::vector<bool>> currentFrame = world;
		for (std::size_t i = 0; i < frameHeight; ++i) {
			const std::size_t u = (i + frameHeight - 1) % frameHeight;
			const std::size_t d = (i + 1) % frameHeight;
			for (std::size_t j = 0; j < frameWidth; ++j) {
				const std::size_t l = (j + frameWidth - 1) % frameWidth;
				const std::size_t r = (j + 1) % frameWidth;
				const std::size_t n = world[u][l] + world[u][j] + world[u][r] + world[i][l] + world[i][r] + world[d][l] + world[d][j] + world[d][r];
				if (world[i][j]) {
					if (n < 2) {
						currentFrame[i][j] = false;
					} else if ((n == 2) || (n == 3)) {
						currentFrame[i][j] = true;
					} else if (n > 3) {
						currentFrame[i][j] = false;
					}
				} else if (n == 3) {
					currentFrame[i][j] = true;
				}
			}
		}
		world = currentFrame;

		if (currentFrame != previousFrame) {
			previousFrame.resize(frameHeight);
			for (auto& row : previousFrame) {
				row.resize(frameWidth);
			}
			std::string display;
			for (std::size_t y = 0; y < frameHeight; y += 2) {
				for (std::size_t x = 0; x < frameWidth; ++x) {
					if ((currentFrame[y][x] == previousFrame[y][x]) && (currentFrame[y + 1][x] == previousFrame[y + 1][x])) {
						continue;
					}
					display = display
						+ terminal.stringSetCursorPosition(xieite::streams::Position(static_cast<int>(y / 2), static_cast<int>(x)))
						+ terminal.stringSetForegroundColor(xieite::streams::Color<3>(currentFrame[y][x] * 0xFF, currentFrame[y][x] * 0xFF, currentFrame[y][x] * 0xFF))
						+ terminal.stringSetBackgroundColor(xieite::streams::Color<3>(currentFrame[y + 1][x] * 0xFF, currentFrame[y + 1][x] * 0xFF, currentFrame[y + 1][x] * 0xFF))
						+ "▀"
						+ terminal.stringResetStyles();
				}
			}
			previousFrame = currentFrame;
			std::print(terminal.outputFile, "{}", display);
			// std::fflush(terminal.outputFile);
		}

		terminal.setCursorPosition(xieite::streams::Position(0, 0));
		std::println(terminal.outputFile, "{}", tick);
		std::fflush(terminal.outputFile);

		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		const std::string input = terminal.readString();
		switch (input[input.size() - 1]) {
		case 'Q':
		case 'q':
			running = false;
			break;
		// case 'D':
		// case 'd':
		// 	++offsetX;
		// 	break;
		// case 'A':
		// case 'a':
		// 	--offsetX;
		// 	break;
		// case 'W':
		// case 'w':
		// 	--offsetY;
		// 	break;
		// case 'S':
		// case 's':
		// 	++offsetY;
		// 	break;
		}
	}
}
