#pragma once
#include <ostream>
#include "player.h"
#include <vector>

using namespace std;
/**
 * This is example player, that plays absolutely randomly.
 */
class RandomPlayer : public Player {
    std::string _name;
public:
    RandomPlayer(const std::string& name): _name(name) {}
    std::string get_name() const override { return _name; }
    Point play(const GameView& game) override;
    void assign_mark(Mark player_mark) override { /*does nothing*/ }
    void notify(const GameView&, const Event&) override { /*does nothing*/ }
};


/**
 * Simple observer, that logs every event in given output stream.
 */
class BasicObserver : public Observer {
    std::ostream& _out;

    std::ostream& _print_mark(Mark mark);
public:

    BasicObserver(std::ostream& out_stream): _out(out_stream) {}
    void notify(const GameView&, const Event& event) override;
};


class MyPlayer : public Player {

    vector<vector<Point>> possibleScenarios;
    Mark myMark;
    Mark enemyMark;

    string _name;
 
    //«аполнение possible_scenarios на основне нынешнего пол€(field) и win_lenght.
    void fillScenarios(vector<vector<Point>>& , const GameSettings&);

    // акой знак нужно поставить в эту точку чтобы победить. MyMark в приритете. 
    //Mark::None если точка не выиграшна€.
    Mark getWinningMark(Point& move, const GameView& game);

    //ѕолучение нужной точки на основе possible_scenarios.
    Point getPossibleScenariosPoint(std::vector<std::vector<Point>>&, const GameSettings&, const GameState&);

public:

    MyPlayer(const std::string& name) : _name(name) {}
    string get_name() const override { return _name; }

    Point play        (const GameView& game)          override;
    void  assign_mark(Mark player_mark)               override;
    void  notify      (const GameView&, const Event&) override;
};

