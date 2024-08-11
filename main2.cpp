#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <print>
#include <thread>
#include <vector>

#include "../xieite/include/xieite/containers/initialize_multidimensional_array.hpp"
#include "../xieite/include/xieite/streams/color.hpp"
#include "../xieite/include/xieite/streams/position.hpp"
#include "../xieite/include/xieite/streams/standard_handle.hpp"

inline constexpr std::size_t textureHeight = 8;
inline constexpr std::size_t textureWidth = 8;
inline constexpr auto textures = xieite::containers::initializeMultidimensionalArray<xieite::streams::Color<4>, 2, textureHeight, textureWidth>({
	{
		{ 0x636363FF, 0x636363FF, 0x636363FF, 0x707070FF, 0x7C7C7CFF, 0x707070FF, 0x707070FF, 0x636363FF },
		{ 0x636363FF, 0x707070FF, 0x8C8C8CFF, 0x7C7C7CFF, 0x707070FF, 0x7C7C7CFF, 0x8C8C8CFF, 0x707070FF },
		{ 0x707070FF, 0x707070FF, 0x7C7C7CFF, 0x8C8C8CFF, 0x7C7C7CFF, 0x8C8C8CFF, 0x707070FF, 0x636363FF },
		{ 0x8C8C8CFF, 0x7C7C7CFF, 0x8C8C8CFF, 0x9C9C9CFF, 0x8C8C8CFF, 0x9C9C9CFF, 0x7C7C7CFF, 0x636363FF },
		{ 0x707070FF, 0x8C8C8CFF, 0x7C7C7CFF, 0x9C9C9CFF, 0x9C9C9CFF, 0x8C8C8CFF, 0x7C7C7CFF, 0x707070FF },
		{ 0x636363FF, 0x7C7C7CFF, 0x9C9C9CFF, 0x8C8C8CFF, 0x7C7C7CFF, 0x7C7C7CFF, 0x8C8C8CFF, 0x707070FF },
		{ 0x636363FF, 0x707070FF, 0x8C8C8CFF, 0x7C7C7CFF, 0x8C8C8CFF, 0x707070FF, 0x7C7C7CFF, 0x636363FF },
		{ 0x707070FF, 0x636363FF, 0x707070FF, 0x636363FF, 0x707070FF, 0x707070FF, 0x636363FF, 0x636363FF }
	},
	{
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF },
		{ 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF },
		{ 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
		{ 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
		{ 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
		{ 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
		{ 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF },
		{ 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF }
	}
});

using Texture = std::array<std::array<xieite::streams::Color<4>, textureWidth>, textureHeight>;

int main() {
	auto terminal = xieite::streams::StandardHandle(stdin, stdout);

	terminal.setInputEcho(false);
	terminal.setInputSignals(false);

	terminal.setScreenAlternate(true);
	terminal.setCursorAlternate(true);
	terminal.setCursorInvisible(true);

	terminal.clearScreen();

	std::vector<std::vector<xieite::streams::Color<4>>> previousFrame;

	std::size_t offsetX = 0;
	std::size_t offsetY = 0;

	bool running = true;
	for (std::size_t tick = 0; running; ++tick) {
		const xieite::streams::Position size = terminal.getScreenSize();
		const std::size_t frameHeight = static_cast<std::size_t>(size.row) * 2;
		const std::size_t frameWidth = static_cast<std::size_t>(size.column);

		std::vector<std::vector<xieite::streams::Color<4>>> currentFrame;
		currentFrame.resize(frameHeight);
		for (auto& row : currentFrame) {
			row.resize(frameWidth);
		}

		const auto drawTexture = [frameHeight, frameWidth, &currentFrame](const Texture& texture, const std::size_t offsetX, const std::size_t offsetY) {
			for (std::size_t y1 = 0; y1 < textureHeight; ++y1) {
				for (std::size_t x1 = 0; x1 < textureWidth; ++x1) {
					const std::size_t y2 = offsetY + y1;
					const std::size_t x2 = offsetX + x1;
					if ((y2 >= frameHeight) || (x2 >= frameWidth) || !texture[y1][x1][3]) {
						continue;
					}
					currentFrame[y2][x2] = texture[y1][x1];
				}
			}
		};

		drawTexture(textures[0], offsetX, offsetY);
		drawTexture(textures[1], offsetX, offsetY);

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
						+ terminal.stringSetForegroundColor(currentFrame[y][x])
						+ terminal.stringSetBackgroundColor(currentFrame[y + 1][x])
						+ "▀"
						+ terminal.stringResetStyles();
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
		case 'D':
		case 'd':
			++offsetX;
			break;
		case 'A':
		case 'a':
			--offsetX;
			break;
		case 'W':
		case 'w':
			--offsetY;
			break;
		case 'S':
		case 's':
			++offsetY;
			break;
		}
	}
}
