#include <iostream>
#include <fstream>
#include <SDL2\SDL.h>
#include "SDL2\SDL_Main.h"
#include <SDL2\SDL_ttf.h>
#include <vector>
#include <cmath>

const int width = 800;
const int height = 800;

int sine(int speed, int time, int how_far, int overall) {
	double t = SDL_GetTicks64() / 2 % time;

	double y = sin(t/speed) * how_far + overall;
	return round(y);

}

bool exists(const char *filename)
{
    std::ifstream infile(filename);
    return infile.good();
}

bool is_in_button(SDL_Rect r, int x, int y) {

	if ((x > r.x) && (x < r.x + r.w) && (y > r.y) && (y < r.y + r.h)) {
		return true;
	}

	return false;
}

struct ball {
	SDL_Renderer * renderer;
	double radius;
	SDL_FRect rect;
	void draw();
	void update();
	bool collision(SDL_FRect a);
	bool pixel_collision(int x, int y);
	void drag_on();
	void drag_off();
	void move(int x, int y);
	void make_circle();
	void stop();
	bool ball_collision(ball a);

	std::vector<std::vector<int>> points;
	bool held = false;
	float direction[2] = {0.0, 0.0};
	long last_update = SDL_GetTicks64();
	bool on = true;
};

struct data {
	std::string score;
	void read();
	void write();
};

void data::read() {

	std::ifstream file("data.dat");
	file >> score;

}

void data::write() {

	std::ofstream file("data.dat");
	file << score.c_str();
	
}

// void ball::draw() {
// 	DrawCircle(renderer, rect.x + rect.w/2, rect.y + rect.h/2, radius);
// }

// void ball::draw(SDL_Renderer * renderer, int32_t centreX, int32_t centreY, int32_t radius)

void ball::make_circle() {
	int32_t centreX = rect.x + rect.w/2;
	int32_t centreY = rect.y + rect.h/2;
	const int32_t diameter = (radius * 2);

	std::vector<std::vector<int>> points_;

	int32_t x = (radius - 1);
	int32_t y = 0;
	int32_t tx = 1;
	int32_t ty = 1;
	int32_t error = (tx - diameter);

	while (x >= y)
	{
		points_.push_back({centreX + x, centreY - y});
		points_.push_back({centreX + x, centreY + y});

		points_.push_back({centreX + x + 1, centreY - y});
		points_.push_back({centreX + x + 1, centreY + y});

		points_.push_back({centreX - x, centreY - y});
		points_.push_back({centreX - x, centreY + y});

		points_.push_back({centreX - x + 1, centreY - y});
		points_.push_back({centreX - x + 1, centreY + y});

		points_.push_back({centreX + y, centreY - x + 1});
		points_.push_back({centreX + y, centreY + x + 1});

		points_.push_back({centreX + y, centreY - x});
		points_.push_back({centreX + y, centreY + x});

		points_.push_back({centreX - y, centreY - x + 1});
		points_.push_back({centreX - y, centreY + x + 1});

		points_.push_back({centreX - y, centreY - x});
		points_.push_back({centreX - y, centreY + x});

		if (error <= 0)
		{
			++y;
			error += ty;
			ty += 2;
		}

		if (error > 0)
		{
			--x;
			tx += 2;
			error += (tx - diameter);
		}
	}
	points = points_;
}

void ball::draw() {
	make_circle();
	for (int i = 0; i < points.size(); i++) {
		SDL_RenderDrawPoint(renderer, points[i][0], points[i][1]);
	}
}

void ball::drag_on() {
	held = true;
}

void ball::drag_off() {
	held = false;
}

void ball::stop() {
	held = false;
	on = false;
}

void ball::move(int x, int y) {
	rect.x = x;
	rect.y = y;
}

bool ball::collision(SDL_FRect a) {
	return SDL_HasIntersectionF(&rect, &a);
}

bool ball::ball_collision(ball a) {
	for (int i = 0; i < points.size(); i++) {
		for (int j = 0; j < a.points.size(); j++) {
			if (points[i][0] == a.points[j][0] && points[i][1] == a.points[j][1]) {
				return true;
			}
		}
	}
	return false;
}

bool ball::pixel_collision(int x, int y) {

	if ((x > rect.x) && (x < rect.x + rect.w) && (y > rect.y) && (y < rect.y + rect.h)) {
		return true;
	}

	return false;
}

void ball::update() {
	if (!on) {
		return;
	}
	long delta_time = SDL_GetTicks64() - last_update;
	if (held) {
		last_update = SDL_GetTicks64();
		return;
	}

	if (rect.y + radius*2 >= height) {
		std::cout << "Wall collision" << std::endl;
		rect.y = height-radius*2;
		direction[1] = -direction[1];
	} else if (rect.y <= 0) {
		std::cout << "Wall collision" << std::endl;
		rect.y = 0;
		direction[1] = -direction[1];
	}
	if (rect.x + radius*2 >= width) {
		std::cout << "Wall collision" << std::endl;
		rect.x = width - radius*2;
		direction[0] = -direction[0];
	} else if (rect.x <= 0) {
		std::cout << "Wall collision" << std::endl;
		rect.x = 0;
		direction[0] = -direction[0];
	}

	rect.x += direction[0] * delta_time / 2;
	rect.y += direction[1] * delta_time / 2;
	last_update = SDL_GetTicks64();

	// if (rect.y + radius*2 >= height) {
	// 	if (momentum > 0) {
	// 		momentum -= 0.1 * density;
	// 		rect.y = height - radius*2;
	// 		velocity = (-bounciness * momentum);
	// 		rect.y += velocity;
	// 	} else {
	// 		rect.y = height - radius*2;
	// 		momentum = 0;
	// 	}

	// } else if (rect.y <= 0) {
		
	// 	rect.y = 0;
	// 	velocity = bounciness * (-velocity / 60);
	// 	rect.y += velocity;
	// } else {
	// 	rect.y += velocity;
	// 	velocity += (density * radius) / 500;
	// }

}

// void ball::set_velocity(double y) {
// 	velocity = y;
// }

int main(int argc, char* argv[]) {

	std::cout << "SDL init" << std::endl;
	SDL_Init(SDL_INIT_EVERYTHING);

	std::cout << "Window init" << std::endl;
	SDL_Window *window = SDL_CreateWindow("Keep the ball away", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_ALLOW_HIGHDPI);

	if (NULL == window) {
		std::cout << "Unable to create window: " << SDL_GetError() << std::endl;
		return 1;
	}

	std::cout << "Renderer init" << std::endl;
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	if (NULL == renderer) {
		std::cout << "Unable to create renderer: " << SDL_GetError() << std::endl;
		return 1;
	}

	std::cout << "SDL_ttf init" << std::endl;
	if (TTF_Init() == -1) {
		std::cout << "Unable to init SDL2_ttf: " << SDL_GetError() << std::endl;
		return 1;
	}

	TTF_Font* font = TTF_OpenFont("assets\\font.ttf", 12);
	if (!font) {
		std::cout << "Unable to open font: " << TTF_GetError() << std::endl;
		return 1;
	}

	std::cout << "Setting up" << std::endl;

	int x, y;
	std::vector<ball> balls;
	balls.push_back(ball {
		renderer,
		50,
		SDL_FRect {
			(float)(rand()%50)+50,
			(float)(rand()%50)+50,
			100,
			100
		}
	});

	balls.push_back(ball {
		renderer,
		50,
		SDL_FRect {
			(float)(rand()%100)+700,
			(float)(rand()%100)+200,
			100,
			100
		}
	});

	balls.push_back(ball {
		renderer,
		50,
		SDL_FRect {
			(float)(rand()%50)+700,
			(float)(rand()%50)+700,
			100,
			100
		}
	});

	for (int i = 0; i < balls.size(); i++) {
		float r = 0;
		while (r > -0.1 && r < 0.1) {
			r = ((float)(rand()%20000) - 10000) / 10000;
		}
		balls[i].direction[0] = r;
		r = 0;
		while (r > -0.1 && r < 0.1) {
			r = ((float)(rand()%20000) - 10000) / 10000;
		}
		balls[i].direction[1] = r;
		std::cout << "Ball " << i+1 << " speed: " << balls[i].direction[0] << ", " << balls[i].direction[1] << std::endl;
	}

	int selected_ball = -1;
	int held_offset[2] = {0,0};
	int points = 0;

	int balls_hit[2] = {-1,-1};

	data d;
	if (exists("data.dat")) {
		d.read();
	} else {
		d.score = "0";
	}

	int points_y;

	SDL_Rect restart_button {
		width/2-100,
		height/5*3,
		200,
		75
	};

	SDL_Rect menu_button {
		width/2-100,
		height/5*3+85,
		200,
		75
	};

	SDL_Rect start_button {
		width/2-100,
		height/5*3,
		200,
		75
	};

	bool in_menu = true;

	SDL_Event event;

	bool running = true;

	// long last_update = SDL_GetTicks();
	long restart_time = SDL_GetTicks64();

	while (running) {

		while(SDL_PollEvent(&event)) {

			if (event.type == SDL_QUIT) {
				running = false;
			}

			if(event.type == SDL_MOUSEMOTION)
			{
				// SDL_GetGlobalMouseState(&x,&y);
				SDL_GetMouseState(&x, &y);
			}

			if (event.type == SDL_KEYDOWN) {
				if (event.key.keysym.sym == SDLK_ESCAPE) {
					running = false;
				}
			}

			if (event.type == SDL_MOUSEBUTTONDOWN) {
				if (event.button.button == SDL_BUTTON_LEFT) {

					if (in_menu && is_in_button(start_button, x, y)) {
						in_menu = false;

						balls_hit[0] = -1;
						balls_hit[1] = -1;

						balls.clear();
						balls.push_back(ball {
							renderer,
							50,
							SDL_FRect {
								(float)(rand()%50)+50,
								(float)(rand()%50)+50,
								100,
								100
							}
						});
						balls.push_back(ball {
							renderer,
							50,
							SDL_FRect {
								(float)(rand()%100)+700,
								(float)(rand()%100)+200,
								100,
								100
							}
						});
						balls.push_back(ball {
							renderer,
							50,
							SDL_FRect {
								(float)(rand()%50)+700,
								(float)(rand()%50)+700,
								100,
								100
							}
						});

						for (int i = 0; i < balls.size(); i++) {
							float r = 0;
							while (r > -0.1 && r < 0.1) {
								r = ((float)(rand()%20000) - 10000) / 10000;
							}
							balls[i].direction[0] = r;
							r = 0;
							while (r > -0.1 && r < 0.1) {
								r = ((float)(rand()%20000) - 10000) / 10000;
							}
							balls[i].direction[1] = r;
							std::cout << "Ball " << i+1 << " speed: " << balls[i].direction[0] << ", " << balls[i].direction[1] << std::endl;
						}

						points = 0;
						restart_time = SDL_GetTicks64();
						selected_ball = -1;
						held_offset[0] = 0;
						held_offset[1] = 0;

					}

					if (balls_hit[0] == -1) {
						for (int i = 0; i < balls.size(); i++) {
							if (balls[i].pixel_collision(x, y)) {
								selected_ball = i;
								balls[i].drag_on();

								held_offset[0] = x-(balls[i].rect.x + balls[i].rect.w/2);
								held_offset[1] = y-(balls[i].rect.y + balls[i].rect.h/2);

								break;
							}
						}
					} else {
						if (is_in_button(restart_button, x, y)) {
							balls_hit[0] = -1;
							balls_hit[1] = -1;

							balls.clear();
							balls.push_back(ball {
								renderer,
								50,
								SDL_FRect {
									(float)(rand()%50)+50,
									(float)(rand()%50)+50,
									100,
									100
								}
							});

							balls.push_back(ball {
								renderer,
								50,
								SDL_FRect {
									(float)(rand()%100)+700,
									(float)(rand()%100)+200,
									100,
									100
								}
							});

							balls.push_back(ball {
								renderer,
								50,
								SDL_FRect {
									(float)(rand()%50)+700,
									(float)(rand()%50)+700,
									100,
									100
								}
							});

							for (int i = 0; i < balls.size(); i++) {
								float r = 0;
								while (r > -0.1 && r < 0.1) {
									r = ((float)(rand()%20000) - 10000) / 10000;
								}
								balls[i].direction[0] = r;
								r = 0;
								while (r > -0.1 && r < 0.1) {
									r = ((float)(rand()%20000) - 10000) / 10000;
								}
								balls[i].direction[1] = r;
								std::cout << "Ball " << i+1 << " speed: " << balls[i].direction[0] << ", " << balls[i].direction[1] << std::endl;
							}

							points = 0;
							restart_time = SDL_GetTicks64();
							selected_ball = -1;
							held_offset[0] = 0;
							held_offset[1] = 0;

						} else if (is_in_button(menu_button, x, y)) {

							in_menu = true;

						}
					}


				}
			}
			if (event.type == SDL_MOUSEBUTTONUP) {
				if (event.button.button == SDL_BUTTON_LEFT) {
					if (selected_ball != -1) {
						balls[selected_ball].drag_off();
						selected_ball = -1;
					}
				}
			}

		}

		if (selected_ball != -1 && balls_hit[0] == -1) {
			balls[selected_ball].move((x - balls[selected_ball].rect.w/2) - held_offset[0], (y - balls[selected_ball].rect.h/2) - held_offset[1]);
		}

		if (balls_hit[0] == -1) {
			for (int i = 0; i < balls.size(); i++) {
				for (int j = 0; j < balls.size(); j++) {
					if (j == i) {
						continue;
					}
					if (balls[i].ball_collision(balls[j])) {
						std::cout << "Ball collision" << std::endl;
						balls_hit[0] = i;
						balls_hit[1] = j;
						for (int k = 0; k < balls.size(); k++) {
							balls[k].stop();
						}
						selected_ball = -1;

						if (std::stoi(d.score) < points) {
							d.score = std::to_string(points);
							d.write();
						}

					}
					// if (balls[i].collision(balls[j].rect)) {
					// 	std::cout << "Oh no you lose" << std::endl;
					// 	balls_hit[0] = i;
					// 	balls_hit[1] = j;
					// 	for (int k = 0; k < balls.size(); k++) {
					// 		balls[k].stop();
					// 	}
					// }
				}
			}
		}


		if (!in_menu) {
			for (int i = 0; i < balls.size(); i++) {
				balls[i].update();
			}
		}


		SDL_SetRenderDrawColor(renderer, 247, 247, 247, 255);
		SDL_RenderClear(renderer);

		if (!in_menu) {
			if (balls_hit[0] == -1) {
				SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
				for (int i = 0; i < balls.size(); i++) {
					balls[i].draw();
				}
			} else {
				for (int i = 0; i < balls.size(); i++) {
					if (i == balls_hit[0] || i == balls_hit[1]) {
						SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
					} else {
						SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
					}
					balls[i].draw();
				}
			}

			std::string str = std::to_string(points);

			SDL_Surface* surf = TTF_RenderText_Solid(font, str.c_str(), {0,0,0});
			SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, surf);
			if (balls_hit[0] == -1) {
				points_y = sine(150, 1920, 2, 0);
			}
			SDL_Rect rect {
				(int)(width/2-str.length()*50/2),
				points_y,
				(int)(str.length()*50),
				100
			};

			SDL_RenderCopy(renderer, message, NULL, &rect);
			
			SDL_FreeSurface(surf);
			SDL_DestroyTexture(message);

			if (balls_hit[0] != -1) {

				rect = {
					0,
					0,
					width,
					height
				};

				SDL_SetRenderDrawColor(renderer, 255, 255, 255, 200);
				SDL_RenderFillRect(renderer, &rect);

				str = "You lose!";
				surf = TTF_RenderText_Solid(font, str.c_str(), {0,0,0});
				message = SDL_CreateTextureFromSurface(renderer, surf);
				rect = {
					(int)(width/2-str.length()*50/2),
					sine(150, 1920, 2, height/6*2),
					(int)(str.length()*50),
					150
				};

				SDL_RenderCopy(renderer, message, NULL, &rect);
				
				SDL_FreeSurface(surf);
				SDL_DestroyTexture(message);

				str = "Restart";
				surf = TTF_RenderText_Solid(font, str.c_str(), {0,0,0});
				message = SDL_CreateTextureFromSurface(renderer, surf);
				rect = {
					width/2-75,
					height/5*3,
					150,
					75
				};

				SDL_RenderCopy(renderer, message, NULL, &rect);
				
				SDL_FreeSurface(surf);
				SDL_DestroyTexture(message);

				str = "Menu";
				surf = TTF_RenderText_Solid(font, str.c_str(), {0,0,0});
				message = SDL_CreateTextureFromSurface(renderer, surf);
				rect = {
					width/2-75,
					height/5*3+85,
					150,
					75
				};

				SDL_RenderCopy(renderer, message, NULL, &rect);
				
				SDL_FreeSurface(surf);
				SDL_DestroyTexture(message);

				SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
				SDL_RenderDrawRect(renderer, &restart_button);
				SDL_RenderDrawRect(renderer, &menu_button);

			}

		} else {

			SDL_Surface* surf = TTF_RenderText_Solid(font, "Start", {0,0,0});
			SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, surf);
			SDL_Rect rect {
				width/2-75,
				height/5*3,
				150,
				75
			};

			SDL_RenderCopy(renderer, message, NULL, &rect);
			
			SDL_FreeSurface(surf);
			SDL_DestroyTexture(message);

			surf = TTF_RenderText_Solid(font, "Ball", {0,0,0});
			message = SDL_CreateTextureFromSurface(renderer, surf);
			rect = {
				width/2-200,
				sine(150, 3840, 4, height/5-100),
				300,
				200
			};

			SDL_RenderCopy(renderer, message, NULL, &rect);
			
			SDL_FreeSurface(surf);
			SDL_DestroyTexture(message);

			surf = TTF_RenderText_Solid(font, "Away!", {0,0,0});
			message = SDL_CreateTextureFromSurface(renderer, surf);
			rect = {
				width/2-75,
				sine(150, 3840, 4, height/5+25),
				350,
				200
			};

			SDL_RenderCopy(renderer, message, NULL, &rect);
			
			SDL_FreeSurface(surf);
			SDL_DestroyTexture(message);

			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			SDL_RenderDrawRect(renderer, &start_button);

		}

		SDL_RenderPresent(renderer);
		if (balls_hit[0] == -1) {
			points = (int)(round((SDL_GetTicks64() - restart_time)/500));
		}
		
	}

	std::cout << "Closing font" << std::endl;
	TTF_CloseFont(font);
	
	std::cout << "Destroying window" << std::endl;
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_Quit();
	SDL_Quit();

	return 0;


}
