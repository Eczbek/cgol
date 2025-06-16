#include <chrono>
#include <cstddef>
#include <cstdio>
#include <print>
#include <random>
#include <thread>
#include <vector>

#include <xieite/io/pos.hpp>
#include <xieite/io/term.hpp>
#include <xieite/math/color3.hpp>

using namespace std::literals;

int main() {
	std::vector<std::vector<bool>> world;

	std::mt19937 rng = std::mt19937(std::random_device()());
	std::bernoulli_distribution dist;

	xieite::term terminal;
	terminal.echo(false);
	terminal.canon(false);
	terminal.signal(false);
	terminal.proc(false);
	terminal.cursor_alt(true);
	terminal.screen_alt(true);
	terminal.cursor_invis(true);
	terminal.clear_screen();

	std::vector<std::vector<xieite::color3>> previousFrame;

	bool running = true;
	while (running) {
		const xieite::pos size = terminal.screen_size();
		const std::size_t frameWidth = static_cast<std::size_t>(size.col);
		const std::size_t frameHeight = static_cast<std::size_t>(size.row) * 2;

		world.resize(frameHeight);
		for (auto& row : world) {
			const std::size_t worldWidth = row.size();
			row.resize(frameWidth);
			for (std::size_t j = worldWidth; j < frameWidth; ++j) {
				row[j] = dist(rng);
			}
		}

		std::vector<std::vector<xieite::color3>> currentFrame;
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
				const int n = world[u][l] + world[u][j] + world[u][r] + world[i][l] + world[i][r] + world[d][l] + world[d][j] + world[d][r];
				currentFrame[i][j] = ((n > 1) && (n < 4) && (world[i][j] || (n == 3))) ? 0xFFFFFF : 0x000000;
			}
		}
		for (std::size_t i = 0; i < frameHeight; ++i) {
			for (std::size_t j = 0; j < frameWidth; ++j) {
				world[i][j] = !!currentFrame[i][j].value();
			}
		}

		if (currentFrame != previousFrame) {
			const bool skip = (currentFrame.size() == previousFrame.size()) && (!currentFrame.size() || !previousFrame.size() || (currentFrame[0].size() == previousFrame[0].size()));

			previousFrame.resize(frameHeight);
			for (auto& row : previousFrame) {
				row.resize(frameWidth);
			}
			std::string display;
			for (std::size_t y = 0; y < frameHeight; y += 2) {
				for (std::size_t x = 0; x < frameWidth; ++x) {
					if (skip && (currentFrame[y][x] == previousFrame[y][x]) && (currentFrame[y + 1][x] == previousFrame[y + 1][x])) {
						continue;
					}
					display += terminal.set_cursor_code(static_cast<xieite::ssize_t>(y / 2), static_cast<xieite::ssize_t>(x));
					display += terminal.fg_code(currentFrame[y][x]);
					display += terminal.bg_code(currentFrame[y + 1][x]);
					display += "▀";
					display += terminal.reset_style_code();
				}
			}
			previousFrame = currentFrame;
			std::print(terminal.out, "{}", display);
			std::fflush(terminal.out);
		}

		// std::this_thread::sleep_for(100ms);

		const std::string input = terminal.read_str();
		if (input.size()) {
			switch (input[input.size() - 1]) {
			case 'Q':
			case 'q':
				running = false;
				break;
			}
		}
	}
}
