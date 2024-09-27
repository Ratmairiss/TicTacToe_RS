#include "my_player.h"
#include "basic_fields.h"
#include <cstdlib>


static field_index_t rand_int(field_index_t min, field_index_t max) {
    return min + rand() % (max - min + 1);
}



Point RandomPlayer::play(const GameView& game) {
    Boundary b = game.get_settings().field_size;
    Point result;
    do {
        result = {
            .x = rand_int(b.min.x, b.max.x),
            .y = rand_int(b.min.y, b.max.y),
        };
    } while(game.get_state().field->get_value(result) != Mark::None);
    return result;
}



void MyPlayer::assign_mark(Mark player_mark)
{ 
    myMark = player_mark; 
    enemyMark = player_mark == Mark::Zero ? Mark::Cross : Mark::Zero; 
}


Mark MyPlayer::getWinningMark(Point& move, const GameView& game)
{
    auto  bounds = game.get_state().field->get_current_boundary();
    for (int dx = 0; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {

            if (dx == 0 && (dy == 0 || dy == -1)) continue;

            int   maxMyForward    = 0, maxMyBackward    = 0,
                  maxEnemyForward = 0, maxEnemyBackward = 0;
            bool  isMyUnbroken    = 1, isEnemyUnbroken  = 1;
            Point p;

            p = move;
            do {
                p.x += dx;
                p.y += dy;

                if (isMyUnbroken)
                    ++maxMyForward;
                if (isEnemyUnbroken)
                    ++maxEnemyForward;

                if (!bounds.is_within(p))
                    break;

                if (game.get_state().field->get_value(p) != myMark)
                    isMyUnbroken = 0;
                if (game.get_state().field->get_value(p) != enemyMark)
                    isEnemyUnbroken = 0;

            } while (maxMyForward    <= game.get_settings().win_length && 
                     maxEnemyForward <= game.get_settings().win_length &&
                     (isMyUnbroken || isEnemyUnbroken));

            p = move;
            isMyUnbroken = 1, isEnemyUnbroken = 1;
            do {
                p.x -= dx;
                p.y -= dy;

                if (isMyUnbroken)
                    ++maxMyBackward;
                if (isEnemyUnbroken)
                    ++maxEnemyBackward;

                if (!bounds.is_within(p))
                    break;

                if (game.get_state().field->get_value(p) != myMark)
                    isMyUnbroken = 0;
                if (game.get_state().field->get_value(p) != enemyMark)
                    isEnemyUnbroken = 0;


            } while (maxMyBackward    <= game.get_settings().win_length - maxMyForward    + 1 &&
                     maxEnemyBackward <= game.get_settings().win_length - maxEnemyForward + 1 &&
                     (isMyUnbroken || isEnemyUnbroken));

            if (maxMyForward + maxMyBackward - 1 >= game.get_settings().win_length) {
                return myMark;
            }
            if (maxEnemyForward + maxEnemyBackward - (game.get_settings().win_length <= 3? 1 : 0) 
                >= game.get_settings().win_length) {
                return enemyMark;
            }

        }
    }
    return Mark::None;
}



void MyPlayer::fillScenarios(vector<vector<Point>> & possibleScenarios,  const GameSettings& gameSettings)
{
    Boundary b = gameSettings.field_size;

    // Все возможные горизонтальные линии.
    for (int y = b.min.y; y < b.max.y + 1; y++)
    {
        for (int x0 = b.min.x; x0 + static_cast<int>(gameSettings.win_length - 2) < b.max.x; x0++)
        {
            std::vector<Point> steps;
            for (int x = x0; x < x0 + static_cast<int>(gameSettings.win_length); x++)
            {
                steps.push_back(Point(x, y));
            }

            possibleScenarios.push_back(steps);
        }
    }

    // Все возможные вертикальные линии.
    for (int x = b.min.x; x < b.max.x + 1; x++)
    {
        for (int y0 = b.min.y; y0 + static_cast<int>(gameSettings.win_length - 2) < b.max.y; y0++)
        {
            std::vector<Point> steps;
            for (int y = y0; y < y0 + static_cast<int>(gameSettings.win_length); y++)
            {
                steps.push_back(Point(x, y));
            }

            possibleScenarios.push_back(steps);
        }
    }

    // Все возможные линии под наклоном.
    Point start{ .x = b.min.x, .y = b.min.y };
    for (int x = start.x; x < (b.max.x - static_cast<int>(gameSettings.win_length + 2)); x++)
    {
        for (int y = start.y; y < (b.max.y - static_cast<int>(gameSettings.win_length + 2)); y++)
        {
            std::vector<Point> steps;
            for (int i = 0; i < static_cast<int>(gameSettings.win_length); i++)
            {
                steps.push_back(Point(x + i, y + i));
            }

            possibleScenarios.push_back(steps);
        }
    }
    start.x = b.max.x;
    for (int x = start.x; 
         x > (b.min.x + static_cast<int>(gameSettings.win_length - 2)); x--)
    {
        for (int y = start.y; 
             y < (b.max.y - static_cast<int>(gameSettings.win_length + 2)); y++)
        {
            std::vector<Point> steps;
            for (int i = 0; 
                 i < static_cast<int>(gameSettings.win_length); i++)
            {
                steps.push_back(Point(x - i, y + i));
            }

            possibleScenarios.push_back(steps);
        }
    }

    // Случайная сортировка possible_scenarios.
    for (int i = 0; i < possibleScenarios.size(); i++)
    {
        int index1 = rand() % possibleScenarios.size();
        int index2 = rand() % possibleScenarios.size();

        std::vector<Point> tmp    = possibleScenarios[index1];
        possibleScenarios[index1] = possibleScenarios[index2];
        possibleScenarios[index2] = tmp;
    }
}



Point MyPlayer::getPossibleScenariosPoint(std::vector<std::vector<Point>>& possibleScenarios, 
                                          const GameSettings& gameSettings, 
                                          const GameState& gameState)
{
    Point    result;
    Boundary b = gameSettings.field_size;

    // Проверяем актуальность нынешнего сценария.
    for (int i = 0; i < gameSettings.win_length; i++)
    {
        if (possibleScenarios.empty()) // Если возможных сценариев 
                                       // не осталось выдаем случайную 
                                       // незанятую точку.
        {
            do {
                result = {
                    .x = rand_int(b.min.x, b.max.x),
                    .y = rand_int(b.min.y, b.max.y),
                };
            } while (gameState.field->get_value(result) != Mark::None);
            return result;
        }

        if (gameState.field->get_value(possibleScenarios[0][i])
            == (myMark == Mark::Cross ? Mark::Zero : Mark::Cross))
        {
            possibleScenarios.erase(possibleScenarios.begin()); // Если на возможном пути 
            i = -1;                                             // противник поставил свой 
                                                                // знак, удаляем этот 
                                                                // возможный путь.
        }
    }

    // Следуем сценарию. При этом выбираем случайную 
    // точку из сценария, чтобы не было так очевидно.
    do {
        result = (*possibleScenarios.begin()) [rand_int(0, gameSettings.win_length - 1)];
    } while (gameState.field->get_value(result) != Mark::None);
    return result;
}



Point MyPlayer::play(const GameView& game)
{
    Point result;

    const GameSettings& gameSettings = game.get_settings();
    const GameState&    gameState    = game.get_state();

    Boundary b = gameSettings.field_size;
    
    // При первом ходе заполняем вектор possible_scenarios, который состоит 
    // из возможных сценариев (Векторов точек).
    if (gameState.number_of_moves <= 1) 
        fillScenarios(possibleScenarios, gameSettings);
    
    // Проверяем есть ли на поле выиграшная точка.
    pair<Point,bool> enemyWinPoint(Point(),false);
    for (int i = b.min.y; i < b.max.y + 1; i++)
        for (int j = b.min.x; j < b.max.x + 1; j++)
        {
            result = { .x = j, .y = i };
            if (gameState.field->get_value(result) == Mark::None)
            {
                Mark winningMark = getWinningMark(result, game);

                if (winningMark == myMark)         //  Если в данной точке победным является 
                    return result;                 // наш знак сразу ставим его.
                else if (winningMark == enemyMark) //  Если в данной точке победным
                {                                  // явлется знак соперника, то пока
                    enemyWinPoint.first = result;  // просто сохраняем точку до конца цикла.
                    enemyWinPoint.second = true;   // (Ведь все еще есть шанс что на поле
                }                                  // есть наша победная точка)
            }                                      
        }                                         
    if (enemyWinPoint.second)
        return enemyWinPoint.first;       

    // Если нет точки на поле, благодаря, которой можно выиграть самому, 
    // или может выиграть враг, следуем сценарию.
    return getPossibleScenariosPoint(possibleScenarios, gameSettings, gameState);
}



void BasicObserver::notify(const GameView& gameview, const Event& event) {
    if (event.get_type() == MoveEvent::TYPE) {
        auto &data = get_data<MoveEvent>(event);
        _out << "Move:\tx = " <<  data.point.x 
            << ",\ty = " << data.point.y << ":\t";
        _print_mark(data.mark) << '\n';
        return;
    }
    if (event.get_type() == PlayerJoinedEvent::TYPE) {
        auto &data = get_data<PlayerJoinedEvent>(event);
        _out << "Player '" << data.name << "' joined as ";
        _print_mark(data.mark) << '\n';
        return;
    }
    if (event.get_type() == GameStartedEvent::TYPE) {
        _out << "Game started\n";
        return;
    }
    if (event.get_type() == WinEvent::TYPE) {
        auto &data = get_data<WinEvent>(event);
        _out << "Player playing ";
        _print_mark(data.winner_mark) << " has won\n";
        return;
    }
    if (event.get_type() == DrawEvent::TYPE) {
        auto &data = get_data<DrawEvent>(event);
        _out << "Draw happened, reason: '" << data.reason << "'\n";
        return;
    }
    
    if (event.get_type() == DisqualificationEvent::TYPE) {
        auto &data = get_data<DisqualificationEvent>(event);
        _out << "Player playing ";
        _print_mark(data.player_mark) << " was disqualified, reason: '" 
            << data.reason << "'\n";
        return;
    }
    
}



std::ostream& BasicObserver::_print_mark(Mark m) {
    if (m == Mark::Cross) return _out << "X";
    if (m == Mark::Zero) return _out << "O";
    return _out << "?";
}



void MyPlayer::notify(const GameView&, const Event& event)
{
    if (event.get_type() == WinEvent::TYPE 
       || event.get_type() == DrawEvent::TYPE) 
    {
        possibleScenarios.clear(); // В случае если игра окончена, 
        return;                    // очищаем все возможные сценарии.
    }
}




