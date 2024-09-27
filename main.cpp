/**
 *  This is example of game, that you can use for debugging.
 */
#include <cstdlib>
#include <ctime>
#include <iostream>

#include "my_player.h"
#include "game_engine.h"


int main() {
    srand(time(NULL));
    
    
    /// Creates observer to log every event to std::cout
    BasicObserver obs(std::cout);

    /// Create setting for game
    GameSettings settings = {
        .field_size = {
            .min = {.x = -1, .y = -1},
            .max = {.x =  1, .y = 1},
        },
        .max_moves = 0,
        .win_length = 3,
        .move_timelimit_ms = 0,
        .isUsingFieldShow = 1,
    };
    
    /// Creates game engine with given settings
    GameEngine game(settings);

    /// Adds observer to the view of this game engine
    game.get_view().add_observer(obs);
    /// Creates first player
    RandomPlayer player1("Vasya");
    /// Adds it to play Xs
    game.set_player(player1);
    /// Create second player
    MyPlayer player2("Kolya");
    /// Adds it to play Os
    game.set_player(player2);
    /// Starts game until someone wins.
    game.play_game();
    

    
    // Test with emulating several games between two custom players.
    int FirstPlayerWins = 0, SecondPlayerWins = 0, Draws = 0;
    for (int i = 0; i < 1000; i++)
    {
        GameSettings settings = {
            .field_size = {
                .min = {.x = -1, .y = -1},
                .max = {.x = 1, .y = 1}, },
            .max_moves = 0,
            .win_length = 3,
            .move_timelimit_ms = 0,
            .isUsingFieldShow = 0
        };
        GameEngine   game(settings);
        MyPlayer     player1("FirstPlayer");
        MyPlayer     player2("SecondPlayer");
        game.set_player(player1);
        game.set_player(player2);

        if (game.play_game() == Mark::Zero)  SecondPlayerWins++;
        else if (game.play_game() == Mark::Cross) FirstPlayerWins++;
        else    Draws++;
    }

    cout << "FirstPlayerWins  " << FirstPlayerWins << endl
         << "SecondPlayerWins " << SecondPlayerWins   << endl
         << "Draws            " << Draws        << endl;
    
    
    return 0;
}
