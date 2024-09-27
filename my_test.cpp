#include "my_player.h"
#include "game_engine.h"


int main() {
    
    GameSettings settingsTest = {
            .field_size = {
                .min = {.x = -1, .y = -1},
                .max = {.x = 1,  .y =  1}, },
            .max_moves  = 0,
            .win_length = 3,
            .move_timelimit_ms = 0, };

    GameEngine       gameTest(settingsTest);
    const GameState& stateTest = gameTest.get_view().get_state();

    MyPlayer playerTest("Test Tetstovich");
    playerTest.assign_mark(Mark::Zero);

    //Тест 1 (Проверка на то, что MyPlayer каждый раз делает разные ходы 
    //(Проверка сортировки вектора possible_scenarios))
    //////////////////////////////////////////////////
    
    int   nCoincidences   = 30; 
    Point previousPoint = playerTest.play(gameTest.get_view());

    //Чтобы очистить possible_scenarios.
    playerTest.notify(gameTest.get_view(), WinEvent::make(Mark::None));

    for (int i = 0; i < 50; i++)
    {
        Point newPoint = playerTest.play(gameTest.get_view());
        if (newPoint == previousPoint)
        {
            previousPoint = newPoint;
            nCoincidences--;
        }
        playerTest.notify(gameTest.get_view(), WinEvent::make(Mark::None));
    }

    if (nCoincidences < 0) return 1;
    //////////////////////////////////////////////////

    //Тест 2 (Проверка на то, то MyPlayer правильно сделает ход если:
    // 1) 2 нолика в ряд и осталось поставить последний для победы.
    // 2) У врага 2 крестика в ряд. Ему осталось поставить 
    // последний и нужно это предотвратить.
    // 3) Есть 2 ряда. В первом 2 нолика. Во втором 2 кретика. 
    // Нужно поставить в ряд с ноликами и победить.)
    /////////////////////////////////////////////////
    
    // 1) и 2)
    Mark marks[]{ Mark::Zero, Mark::Cross };
    for (int i = 0; i < 2; i++)
    {
        stateTest.field->set_value(Point(-1, -1), marks[i]);
        stateTest.field->set_value(Point(0, 0),   marks[i]);

        if (playerTest.play(gameTest.get_view()) != Point(1, 1))
            return 1;

        stateTest.field->clear();
    }
    
    // 3)
    for (int i = 0; i < 2; i++)
    {
        stateTest.field->set_value(Point(i - 1 + i, -1), marks[i]);
        stateTest.field->set_value(Point(i - 1 + i, 0),  marks[i]);
    }

    if (playerTest.play(gameTest.get_view()) != Point(-1, 1))
        return 1;
    stateTest.field->clear();
    //////////////////////////////////////////////////

    //Тест 3 (Полная симуляция игры и сравнение количества побед MyPlayer, 
    //с победами игрока, который ходит случайно)
    //////////////////////////////////////////////////
    int MyPlayerWins = 0, RandomWins = 0, Draws = 0;
    for (int i = 0; i < 1000; i++)
    {
        GameSettings settings = {
            .field_size = {
                .min = {.x = -2, .y = -2},
                .max = {.x = 2, .y = 2}, },
            .max_moves = 0,
            .win_length = 4,
            .move_timelimit_ms = 0, 
        };
        GameEngine   game(settings);
        RandomPlayer player1("Vasya");
        MyPlayer     player2("Kolya");
        game.set_player(player1);
        game.set_player(player2);

        if      (game.play_game() == Mark::Zero)  MyPlayerWins++;
        else if (game.play_game() == Mark::Cross) RandomWins++;
        else    Draws++;
    }

    if ((MyPlayerWins / 2) < RandomWins || (MyPlayerWins / 2) < Draws)
        return 1;
    //////////////////////////////////////////////////////

    return 0;
}
