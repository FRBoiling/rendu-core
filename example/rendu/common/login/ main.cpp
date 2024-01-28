#include <entt/entt.hpp>
#include <string>
#include <iostream>

struct Player {
  std::string name;
  //玩家id
  int id;
  //玩家等级
  int level;
  // other player-related data...
};

struct Scene {
  std::string name;
  // other scene-related data...
};

struct SceneMembership {
  entt::entity scene;
};


class Game {
private:
  entt::registry registry;
  std::unordered_map<std::string, entt::entity> playerByName;
  std::unordered_map<entt::entity, SceneMembership> sceneByPlayer;
  // other game-related data...


public:
  entt::entity createScene(const std::string &name) {
    auto scene = registry.create();
    registry.emplace<Scene>(scene, name);
    return scene;
  }

  entt::entity createPlayer(const std::string &name) {
    auto player = registry.create();
    registry.emplace<Player>(player, name);
    playerByName[name] = player;
    return player;
  }

  void addPlayerToScene(entt::entity player, entt::entity scene) {
    if (registry.valid(player) && registry.valid(scene)) {
      registry.emplace<SceneMembership>(player, scene);
    }
  }

  Player *getPlayerInSceneByName(entt::entity scene, const std::string &playerName) {
    auto it = playerByName.find(playerName);
    if (it != playerByName.end()) {
      auto entity = it->second;
      if (registry.valid(entity)) {
        auto membership = registry.try_get<SceneMembership>(entity);
        if (membership && membership->scene == scene) {
          std::cout << "Found player in scene: " << playerName << "\n";
          return &registry.get<Player>(entity);
        }
      }
    }
    std::cout << "Player not found in scene.\n";
    return nullptr;
  }
  // other game-related methods...
};

int main() {
  Game game;
  auto scene1 = game.createScene("Scene1");
  auto scene2 = game.createScene("Scene2");
  auto player1 = game.createPlayer("Player1");
  auto player2 = game.createPlayer("Player2");
  game.addPlayerToScene(player1, scene1);
  game.addPlayerToScene(player2, scene2);
  auto player_info1 = game.getPlayerInSceneByName(scene1, "Player1");
  auto player_info2 = game.getPlayerInSceneByName(scene2, "Player2");
  auto player_info3 = game.getPlayerInSceneByName(scene2, "Player3");
  return 0;
}