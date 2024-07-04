#include "macros.h"
#include "includes.h"

int main()
{
    srand(time(NULL));

    #ifdef _DEBUG
        RenderWindow window(VideoMode(SC_width, SC_height), "Tetris");
    #else
        RenderWindow window(VideoMode(SC_width, SC_height), "Tetris", Style::None);
    #endif
    Event event;

    Main_Plain::obj_for_render* ptr_to_current_obj;

    Main_Plain P(window, ptr_to_current_obj);    

    while (window.isOpen())
    {
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
                case sf::Event::Closed:
                {
                    EXIT
                }

                case sf::Event::KeyPressed:
                {
                    switch (event.key.code)
                    {
                        case sf::Keyboard::Escape:
                        {
                            EXIT
                        }

                        case sf::Keyboard::Enter:
                        {
                            P.game_paused = !P.game_paused;
                            break;
                        }

                        case sf::Keyboard::Space:
                        {
                            P.drop_immediately();
                            break;
                        }

                        case sf::Keyboard::Up:
                        {
                            P.change_in_rotation();
                            break;
                        }

                        case sf::Keyboard::Down:
                        {
                            P.move_down();
                            break;
                        }

                        case sf::Keyboard::Left:
                        {
                            P.move_left();
                            break;
                        }

                        case sf::Keyboard::Right:
                        {
                            P.move_right();
                            break;
                        }

                        default:
                        { break; }
                    }
                }

                default:
                { break; }
            }
        }

        // update kontrolek po ka¿dym inpucie
        
        // wyœwietlanie i update zmian tylko po up³ywie czasu

        if (P.check_time() || P.game_paused) continue;

        P.next_iteration();
    }

    return 0;
}