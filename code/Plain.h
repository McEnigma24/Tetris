#pragma once
#include "macros.h"
#include <vector>
#include <time.h>
#include <vector>
#include <functional>
#include <fstream>
#include <string>

Color random_color()
{
	return Color(rand() % (COLOR_RANGE_diff)+COLOR_RANGE_low,
		rand() % (COLOR_RANGE_diff)+COLOR_RANGE_low,
		rand() % (COLOR_RANGE_diff)+COLOR_RANGE_low
	);
}

class matrix
{
public:
	struct cell
	{
		RectangleShape obj_for_visualisation;
		bool empty_space;

		cell() : empty_space(true) {}
	};

	vector<cell> data;
	u64 width;

public:

	matrix(u64 w, u64 h)
		:data(w* h), width(w) { }

	cell& operator()(u16 x, u16 y)
	{
		//cout << x << " " << y << endl;

		return data[x + (width * y)];
	}

	friend class Main_Plain;
};

struct single_object
{
	u8 dim;
	u8 max_rotation;
	u8 how_many_blocks;

	u8* tab;
};

namespace Objects
{
	const static u8 number_of_shapes = 7;

	static u8 linee[2][4][4] = {
								{
									{ 1, 0, 0, 0 },
									{ 1, 0, 0, 0 },
									{ 1, 0, 0, 0 },
									{ 1, 0, 0, 0 }
								},
								{
									{ 1, 1, 1, 1 },
									{ 0, 0, 0, 0 },
									{ 0, 0, 0, 0 },
									{ 0, 0, 0, 0 }
								}
	};

	static u8 dick[4][3][3] = {
								{
									{ 0, 1, 0 },
									{ 1, 1, 1 },
									{ 0, 0, 0 }
								},
								{
									{ 0, 1, 0 },
									{ 0, 1, 1 },
									{ 0, 1, 0 }
								},
								{
									{ 0, 0, 0 },
									{ 1, 1, 1 },
									{ 0, 1, 0 }
								},
								{
									{ 0, 1, 0 },
									{ 1, 1, 0 },
									{ 0, 1, 0 }
								}
	};

	static u8 sq[1][2][2] = {
								{
									{ 1, 1 },
									{ 1, 1 }
								}
	};

	static u8 step_1[2][3][3] = {
								{
									{ 1, 1, 0 },
									{ 0, 1, 1 },
									{ 0, 0, 0 }
								},
								{
									{ 0, 1, 0},
									{ 1, 1, 0},
									{ 1, 0, 0}
								}
	};

	static u8 step_2[2][3][3] = {
								{
									{ 0, 1, 1 },
									{ 1, 1, 0 },
									{ 0, 0, 0 }
								},
								{
									{ 0, 1, 0 },
									{ 0, 1, 1 },
									{ 0, 0, 1 }
								}
	};

	static u8 L_1[4][3][3] = {
								{
									{ 0, 1, 0 },
									{ 0, 1, 0 },
									{ 0, 1, 1 }
								},
								{
									{ 0, 0, 0 },
									{ 1, 1, 1 },
									{ 1, 0, 0 }
								},
								{
									{ 1, 1, 0 },
									{ 0, 1, 0 },
									{ 0, 1, 0 }
								},
								{
									{ 0, 0, 1 },
									{ 1, 1, 1 },
									{ 0, 0, 0 }
								}
	};

	static u8 L_2[4][3][3] = {
								{
									{ 0, 1, 0 },
									{ 0, 1, 0 },
									{ 1, 1, 0 }
								},
								{
									{ 1, 0, 0 },
									{ 1, 1, 1 },
									{ 0, 0, 0 }
								},
								{
									{ 0, 1, 1 },
									{ 0, 1, 0 },
									{ 0, 1, 0 }
								},
								{
									{ 0, 0, 0 },
									{ 1, 1, 1 },
									{ 0, 0, 1 }
								}
	};
}

class Stats
{
	chrono::milliseconds& time_slot;
	u64 cleared_rows;
	u64 how_many_fallen_objects;
	u64 how_many_fallen_blocks;
	bool first;

public:
	Font font;
	Text text;

	Stats(chrono::milliseconds& ts)
		: time_slot(ts), cleared_rows(0), how_many_fallen_objects(0), how_many_fallen_blocks(0), first(true)
	{
		init_text();
	}

	void init_text()
	{
		//if (!font.loadFromFile("./raleway/Raleway-Italic.ttf"))
		if (!font.loadFromFile("Raleway-Italic.ttf"))
		{
			log("font not loaded");
		}
		text.setFont(font);

		text.setFillColor(STATS_color);
		text.setPosition(STATS_pos);
		text.setCharacterSize(STATS_size);
		text.setStyle(sf::Text::Bold);
		text.setString("");
	}

	u64 get_rows_cleared()
	{
		return cleared_rows;
	}

	void row_cleared(u64 number_of_rows = 1)
	{
		cleared_rows += number_of_rows;
		update_stats_text();
	}
	void object_fallen()
	{
		if (first)
		{
			first = false;
			return;
		}
		how_many_fallen_objects++;
		how_many_fallen_blocks += 4;
		update_stats_text();
	}
	void update_stats_text()
	{
		text.setString(
			  "Cleared rows   " + to_string(cleared_rows) + "\n"
			+ "Fallen objects " + to_string(how_many_fallen_objects) + "\n"
			+ "Speed " + to_string(time_slot.count()) + "\n"

			//+ "Fallen blocks  " + to_string(how_many_fallen_blocks) + "\n"
		);
	}
};

class Interpolation
{
	struct INPUT_x_y_pair
	{
		double x;
		double y;
		bool locked;

		INPUT_x_y_pair(double xx, double yy)
			:x(xx), y(yy), locked(false) {}
	};
	vector<INPUT_x_y_pair> config_values;

	void extract_doubles(const string& line)
	{
		istringstream iss(line);
		string first;  // x
		string second; // y

		getline(iss, first, ' ');
		getline(iss, second);

		config_values.push_back(INPUT_x_y_pair(stod(first), stod(second)));
	}
	void read_config()
	{
		fstream FILE(PATH_config);
		string line;

		while (getline(FILE, line))
			extract_doubles(line);

		FILE.close();
	}

public:
	friend class Main_Plain;

	Interpolation()
	{
		read_config();
		//check_interpolation();
	}

	// 4 wartoœci minimum

	bool config_init_good()
	{
		if (config_values.size() >= 4) return true;
		return false;
	}

	double value_in_point(double x)
	{
		u8 how_many_numbers = config_values.size();

		if (how_many_numbers < 4) return TIME_SLOT_default;

		if (x < config_values[0].x ) return config_values[0].y;
		if(config_values[how_many_numbers - 1].x < x) return config_values[how_many_numbers - 1].y;

		u8 index;
		for (index = 1; index < how_many_numbers; index++)
		{
			if (!(config_values[index + 1].x < x && index + 3 != how_many_numbers))
				break;
		}

		const u8 first_index = index - 1;
		const u8 last_index = first_index + 3;

		double all = 0;
		for (u8 locked_index = first_index; locked_index <= last_index; locked_index++)
		{
			auto& current_locked = config_values[locked_index];

			current_locked.locked = true;
			{
				double one_block_upper = current_locked.y;
				double one_block_lower = 1;

				for (u8 index = first_index; index <= last_index; index++)
				{
					auto& current = config_values[index];

					if (!current.locked)
					{
						one_block_upper *= (x - current.x);
						one_block_lower *= (current_locked.x - current.x);
					}
				}

				all += one_block_upper / one_block_lower;
			}
			current_locked.locked = false;
		}

		return all;
	}

	void check_interpolation()
	{
		ofstream FILE("output.txt");

		FILE.precision(4);
		for (double x0 = 1.0; x0 <= 100.0; x0 += 0.1)
		{
			FILE << fixed << x0 << " " << value_in_point(x0) << endl;
		}

		FILE.close();
	}
};

class Main_Plain
{
public:
	struct obj_for_render
	{
		struct coord_2d
		{
			u8 x;
			u8 y;

			coord_2d() :x(0), y(0) {}
		};

		coord_2d position_of_top_corner_on_screen;
		u8 current_rotation;
		Color color;

		single_object* obj;

		obj_for_render() :position_of_top_corner_on_screen(),
			current_rotation(0), color(random_color()),
			obj(nullptr) {}

		u8 operator()(u16 x, u16 y) const
		{
			return obj->tab[x + (obj->dim * y) + current_rotation * (obj->dim * obj->dim)];
		}
	};

	chrono::steady_clock::time_point timer_from_beggining;
	chrono::milliseconds time_slot;
	bool game_paused;

private:
	RenderWindow& window;
	u16 row;
	u16 col;
	matrix plain;
	single_object all_objects[Objects::number_of_shapes];
	obj_for_render current_obj;
	Interpolation interpolation;
	Stats stats;

public:

	Main_Plain(RenderWindow& w, obj_for_render*& main_obj)
		:window(w), row(SC_height / sq_leni + HIDDEN_ARRAY_SIZE), col(SC_width / sq_leni), plain(col, row),
		timer_from_beggining(chrono::steady_clock::now()),
		time_slot(static_cast<chrono::milliseconds>(TIME_SLOT_default)),
		game_paused(false),
		interpolation(), stats(time_slot)
	{
		init_objects();
		init_rectangles();
		assign_pointer(main_obj);

		assign_new_falling_object();

		if (interpolation.config_init_good())
		{
			update_speed();
		}
	}
	void init_rectangles()
	{
		Vector2f pos;

		for (u16 y = 0; y < HIDDEN_ARRAY_SIZE; y++)
		{
			for (u16 x = 0; x < col; x++)
			{
				plain(x, y).empty_space = true;

				if (x == 0 || x == col - 1)
					plain(x, y).empty_space = false;
			}
		}

		for (u16 y = HIDDEN_ARRAY_SIZE - 1; y < row; y++)
		{
			for (u16 x = 0; x < col; x++)
			{
				RectangleShape& current = plain(x, y).obj_for_visualisation;
				plain(x, y).empty_space = true;

				pos.x = static_cast<float>(x * sq_leni);
				pos.y = static_cast<float>((y - HIDDEN_ARRAY_SIZE) * sq_leni);

				current.setSize(square);
				current.setPosition(pos);

				if ((x == 0) || (x == col - 1 || y == row - 1))
				{
					plain(x, y).empty_space = false;
					current.setFillColor(COLOR_borders);
				}
				else
					current.setFillColor(COLOR_background);
					//current.setFillColor(random_color());
			}
		}
	}
	void init_objects()
	{
		using namespace Objects;

		// line
		{
			single_object& current = all_objects[0];
			
			current.dim = 4;
			current.max_rotation = 2;
			current.tab = &linee[0][0][0];
		}

		// dick
		{
			single_object& current = all_objects[1];
			
			current.dim = 3;
			current.max_rotation = 4;
			current.tab = &dick[0][0][0];
		}

		// sq
		{
			single_object& current = all_objects[2];

			current.dim = 2;
			current.max_rotation = 1;
			current.tab = &sq[0][0][0];
		}

		// step_1
		{
			single_object& current = all_objects[3];

			current.dim = 3;
			current.max_rotation = 2;
			current.tab = &step_1[0][0][0];
		}

		// step_2
		{
			single_object& current = all_objects[4];

			current.dim = 3;
			current.max_rotation = 2;
			current.tab = &step_2[0][0][0];
		}

		// L_1
		{
			single_object& current = all_objects[5];

			current.dim = 3;
			current.max_rotation = 4;
			current.tab = &L_1[0][0][0];
		}

		// L_2
		{
			single_object& current = all_objects[6];

			current.dim = 3;
			current.max_rotation = 4;
			current.tab = &L_2[0][0][0];
		}
	}
	void assign_pointer(obj_for_render*& main_obj)
	{
		main_obj = &(current_obj);
	}
	void output_empty_spaces_in_plane()
	{
		string log;

		for (u16 y = 0; y < row; y++)
		{
			log += to_string(y) + " - ";
			for (u16 x = 0; x < col; x++)
			{
				auto& current = plain(x, y).empty_space;
				log += to_string(!current);
			}
			log += '\n';
		}

		cout << log;
		cout << endl;
		cout << endl;
	}
	bool check_time()
	{
		auto now = chrono::steady_clock::now();
		auto difference = chrono::duration_cast<chrono::milliseconds>(now - timer_from_beggining);

		if (difference < time_slot)
			return true;

		timer_from_beggining = now;
		return false;
	}
	void assign_new_falling_object()
	{
		current_obj.obj = &all_objects[rand() % Objects::number_of_shapes];
		current_obj.current_rotation = rand() % current_obj.obj->max_rotation;

		current_obj.color = random_color();

		current_obj.position_of_top_corner_on_screen.x = col / 2 - 1;
		current_obj.position_of_top_corner_on_screen.y = 0;

		// STATS
		stats.object_fallen();
	}
	void display_stats()
	{
		//cout << (string)stats.text.getString() << endl;
		window.draw(stats.text);
	}


	void next_iteration()
	{
		update_positions();

		update_screen();
	}


	void update_positions()
	{
		current_obj.position_of_top_corner_on_screen.y++;

		if (!move_is_possible())
		{
			stop_current_obj_in_its_previous_position();
			clear_all_complete_rows_and_drop_remaining();
			assign_new_falling_object();
		}
	}

	void update_screen()
	{
		window.clear();
		{
			// FINDING shadow
			u16 shadow_y = find_last_possible_position_for_obj();

			// MARKING shadow & player
			for (u16 y = 0; y < current_obj.obj->dim; y++)
			{
				for (u16 x = 0; x < current_obj.obj->dim; x++)
				{
					if (current_obj(x, y) == 1)
					{
						{
							int X = current_obj.position_of_top_corner_on_screen.x + x;
							int Y = shadow_y + y;

							matrix::cell& current = plain(X, Y);
							current.obj_for_visualisation.setFillColor(COLOR_shadow);
						}

						{
							int X = current_obj.position_of_top_corner_on_screen.x + x;
							int Y = current_obj.position_of_top_corner_on_screen.y + y;

							matrix::cell& current = plain(X, Y);
							current.obj_for_visualisation.setFillColor(current_obj.color);
						}
					}
				}
			}

			// DRAW ON SCREEN
			for (u16 y = HIDDEN_ARRAY_SIZE - 1; y < row; y++)
			{
				for (u16 x = 0; x < col; x++)
				{
					matrix::cell& current = plain(x, y);

					window.draw(current.obj_for_visualisation);
				}
			}

			// CLEARING
			for (u16 y = 0; y < current_obj.obj->dim; y++)
			{
				for (u16 x = 0; x < current_obj.obj->dim; x++)
				{
					if (current_obj(x, y) == 1)
					{
						int X = current_obj.position_of_top_corner_on_screen.x + x;
						int Y = current_obj.position_of_top_corner_on_screen.y + y;

						{
							matrix::cell& current = plain(X, Y);
							current.obj_for_visualisation.setFillColor(COLOR_background);
						}

						X = current_obj.position_of_top_corner_on_screen.x + x;
						Y = shadow_y + y;
						{
							matrix::cell& current = plain(X, Y);
							current.obj_for_visualisation.setFillColor(COLOR_background);
						}
					}
				}
			}

			// STATS
			display_stats();
		}
		window.display();
	}

	void update_speed()
	{
		double iv_double = interpolation.value_in_point(stats.get_rows_cleared());
		u64 iv_int = static_cast<u64>(iv_double);

		time_slot = static_cast<chrono::milliseconds>(iv_int);
	}



	// UPDATE SCREEN
	void iteratio_on_current_obj(std::function<void(matrix::cell&)> func)
	{
		for (u16 y = 0; y < current_obj.obj->dim; y++)
		{
			for (u16 x = 0; x < current_obj.obj->dim; x++)
			{
				if (current_obj(x, y) == 1)
				{
					int X = current_obj.position_of_top_corner_on_screen.x + x;
					int Y = current_obj.position_of_top_corner_on_screen.y + y;
					matrix::cell& current = plain(X, Y);

					func(current);
				}
			}
		}
	}
	void iteratio_on_current_obj(std::function<void(matrix::cell&, Main_Plain::obj_for_render&)> func)
	{
		for (u16 y = 0; y < current_obj.obj->dim; y++)
		{
			for (u16 x = 0; x < current_obj.obj->dim; x++)
			{
				if (current_obj(x, y) == 1)
				{
					int X = current_obj.position_of_top_corner_on_screen.x + x;
					int Y = current_obj.position_of_top_corner_on_screen.y + y;
					matrix::cell& current = plain(X, Y);

					func(current, current_obj);
				}
			}
		}
	}
	void iteratio_on_current_obj_finding_shadow(std::function<void(matrix::cell&)> func)
	{
		for (u16 y = 0; y < current_obj.obj->dim; y++)
		{
			for (u16 x = 0; x < current_obj.obj->dim; x++)
			{
				if (current_obj(x, y) == 1)
				{
					int X = current_obj.position_of_top_corner_on_screen.x + x;
					int Y = current_obj.position_of_top_corner_on_screen.y + y;
					matrix::cell& current = plain(X, Y);

					func(current);
				}
			}
		}
	}
	u16 find_last_possible_position_for_obj()
	{
		for (u16 yy = 0; ; yy++)
		{
			if (!move_is_possible(current_obj.position_of_top_corner_on_screen.x, current_obj.position_of_top_corner_on_screen.y + yy))
			{
				return current_obj.position_of_top_corner_on_screen.y + yy - 1;
			}
		}
	}

	// UPDATE POSITIONS
	u16 count_elements_in_row(u16 y)
	{
		u16 ret = 0;

		for (u16 x = 1; x < col - 1; x++)
		{
			auto& current = plain(x, y);

			if (!current.empty_space)
				ret++;
		}

		return ret;
	}
	bool is_row_with_elements(u16 y)
	{
		return count_elements_in_row(y);
	}
	bool is_row_empty(u16 y)
	{
		return !count_elements_in_row(y);
	}
	vector<u16> is_there_any_completed_row()
	{
		vector<u16> ret;
		u16 counter;

		for (u16 y = row - 2; y >= 2; y--)
		{
			counter = count_elements_in_row(y);

			//VAR(counter)

			if (counter == 0)
			{
				ret.push_back(y);
				break;
			}
			else if (counter == col - 2)
			{
				ret.push_back(y);
			}
		}

		return ret;
	}
	void clearing_all_rows(vector<u16>& list_of_full_rows)
	{
		// STATS
		stats.row_cleared(list_of_full_rows.size());
		update_speed();

		for (auto& y : list_of_full_rows)
		{
			clear_single_row(y);
		}
	}
	void clear_single_row(u16 y)
	{
		for (u16 x = 1; x < col - 1; x++)
		{
			auto& current = plain(x, y);
			current.obj_for_visualisation.setFillColor(COLOR_background);
			current.empty_space = true;
		}
	}
	void copy_single_row_and_reset_old_place(u16 source, u16 destination)
	{
		for (u16 x = 1; x < col - 1; x++)
		{
			auto& og = plain(x, source);
			auto& new_one = plain(x, destination);

			new_one.obj_for_visualisation.setFillColor(og.obj_for_visualisation.getFillColor());
			new_one.empty_space = og.empty_space;

			og.obj_for_visualisation.setFillColor(COLOR_background);
			og.empty_space = true;
		}
	}
	void clear_all_complete_rows_and_drop_remaining()
	{
		auto list_of_full_rows = is_there_any_completed_row();
		auto size = list_of_full_rows.size();

		if (size <= 1) return;

		u16 from_bottom_first_empty_row = list_of_full_rows[size - 1] - 1;
		u16 top_row_to_be_cleared = list_of_full_rows[size - 2] - 1;
		u16 bottom_row_to_be_cleared = list_of_full_rows[0] - 1;

		list_of_full_rows.pop_back();

		clearing_all_rows(list_of_full_rows);

		if (top_row_to_be_cleared - 1 != from_bottom_first_empty_row)
		{
			// jest coœ na górze

			for (u16 y = row - 2; y >= 0; y--)
			{
				if (y == 1)
				{
					// GAME OVER

					log("GAME OVER");
					break;
				}

				if (y == from_bottom_first_empty_row)
					break;

				if (is_row_with_elements(y))
					continue;

				for (u16 searching_row = y - 1;; searching_row--)
				{
					if (searching_row == from_bottom_first_empty_row)
					{
						// nie ma ju¿ ¿adnych pe³nych row powy¿ej
						break;
					}

					if (is_row_with_elements(searching_row))
					{
						copy_single_row_and_reset_old_place(searching_row, y);
						break;
					}
				}
			}
		}

		/*
		else
		{
			u16 chunk_to_fall_bottom_y = top_row_to_be_cleared - 1;
			u16 chunk_to_fall_top_y = from_bottom_first_empty_row + 1;

			u16 move_this_much_lower = bottom_row_to_be_cleared - top_row_to_be_cleared + 1;

			copy_to_new_space_and_clear_old(chunk_to_fall_bottom_y, chunk_to_fall_top_y, move_this_much_lower);
		}*/
	}
	void stop_current_obj_in_its_previous_position()
	{
		current_obj.position_of_top_corner_on_screen.y--;

		iteratio_on_current_obj([](matrix::cell& current, Main_Plain::obj_for_render& current_obj) -> void
			{
				current.obj_for_visualisation.setFillColor(current_obj.color);
				current.empty_space = false;
				return;
			});
	}
	bool move_is_possible()
	{
		// przejœæ po wszystkich miejscach figury i sprawdziæ czy nie nachodzi na !empty_space

		for (u16 y = 0; y < current_obj.obj->dim; y++)
		{
			for (u16 x = 0; x < current_obj.obj->dim; x++)
			{
				if (current_obj(x, y) == 1)
				{
					int X = current_obj.position_of_top_corner_on_screen.x + x;
					int Y = current_obj.position_of_top_corner_on_screen.y + y;

					matrix::cell& current = plain(X, Y);

					if (!current.empty_space)
					{
						return false;
					}
				}
			}
		}

		return true;
	}
	bool move_is_possible(u8 pos_x, u8 pos_y)
	{
		// przejœæ po wszystkich miejscach figury i sprawdziæ czy nie nachodzi na !empty_space

		for (u16 y = 0; y < current_obj.obj->dim; y++)
		{
			for (u16 x = 0; x < current_obj.obj->dim; x++)
			{
				if (current_obj(x, y) == 1)
				{
					int X = pos_x + x;
					int Y = pos_y + y;

					matrix::cell& current = plain(X, Y);

					if (!current.empty_space)
					{
						return false;
					}
				}
			}
		}

		return true;
	}

	// USER INPUT
	#define going_back_if_needed(var, operation) \
	{\
		u8 prev = var;\
		operation;\
		if (!move_is_possible())\
		{ \
			var = prev;	\
			timer_from_beggining = chrono::steady_clock::now(); \
		} \
		else \
			update_screen(); \
	}\

	void change_in_rotation()
	{
		going_back_if_needed(current_obj.current_rotation, current_obj.current_rotation = (current_obj.current_rotation + 1) % current_obj.obj->max_rotation;);
	}
	void move_left()
	{
		going_back_if_needed(current_obj.position_of_top_corner_on_screen.x, current_obj.position_of_top_corner_on_screen.x--);
	}
	void move_right()
	{
		going_back_if_needed(current_obj.position_of_top_corner_on_screen.x, current_obj.position_of_top_corner_on_screen.x++);
	}
	void move_down()
	{
		going_back_if_needed(current_obj.position_of_top_corner_on_screen.y, current_obj.position_of_top_corner_on_screen.y++);
	}
	void drop_immediately()
	{
		u16 shadow_y = find_last_possible_position_for_obj();

		current_obj.position_of_top_corner_on_screen.y = shadow_y;
		next_iteration();
	}
};