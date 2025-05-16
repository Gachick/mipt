#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

class Group;

class User {
public:
  User(int userId, std::string username, std::string description)
      : userId(userId), username(username), description(description),
        group(std::shared_ptr<Group>()) {};

  User(int userId, std::string username, std::string description,
       std::shared_ptr<Group> group)
      : userId(userId), username(username), description(description),
        group(group) {};

  void setGroup(std::shared_ptr<Group> newGroup);
  void getInfo() const;

private:
  int userId;
  std::string username;
  std::string description;
  std::weak_ptr<Group> group;
};

class Group {
public:
  Group(int groupId) : groupId(groupId) {}

  void addUser(std::shared_ptr<User> user);

  int getGroupId() const;

  void getInfo() const;

private:
  int groupId;
  std::vector<std::weak_ptr<User>> users;
};

void Group::getInfo() const {
  std::cout << "Group id: " << groupId << "\n";
  for (auto user : users) {
    if (std::shared_ptr<User> u = user.lock()) {
      u->getInfo();
    }
  }
}

void User::setGroup(std::shared_ptr<Group> newGroup) { group = newGroup; }

void User::getInfo() const {
  std::cout << "User id: " << userId << "\n"
            << "User name: " << username << "\n"
            << "User description: " << description << "\n";
  if (std::shared_ptr<Group> g = group.lock()) {
    std::cout << "This User is a member of group " << g->getGroupId() << "\n";
  }
  std::cout << "\n";
}

int Group::getGroupId() const { return groupId; }

void Group::addUser(std::shared_ptr<User> user) { users.push_back(user); }

class UserManager {
public:
  std::shared_ptr<User> createUser(int userId, std::string username,
                                   std::string description) {
    if (users.count(userId)) {
      std::cout << "\nuser with id " << userId << " already exists\n\n";
      return nullptr;
    }

    std::shared_ptr<User> user =
        std::make_shared<User>(userId, username, description);
    users.insert(std::make_pair(userId, user));

    std::cout << "User with id " << userId << " was successfully created\n\n";
    return user;
  }

  std::shared_ptr<User> createUser(int userId, std::string username,
                                   std::string description, int groupId) {
    auto group = findGroup(groupId);

    if (!group) {
      return nullptr;
    }

    auto user = createUser(userId, username, description);

    if (!user) {
      return nullptr;
    }

    user->setGroup(group);
    group->addUser(user);

    return user;
  }

  void getUser(int userId) {
    if (auto user = findUser(userId)) {
      user->getInfo();
    }
  }

  void deleteUser(int userId) {
    if (auto user = findUser(userId)) {
      users.erase(userId);
      std::cout << "User with id " << userId << " was successfully deleted\n\n";
    }
  }

  std::shared_ptr<User> findUser(int userId) {
    auto user = users.find(userId);
    if (user == users.end()) {
      std::cout << "\nuser with id " << userId << " doesn't exists\n\n";
      return nullptr;
    }
    return user->second;
  }

  void allUsers() const {
    for (const auto &user_pair : users) {
      user_pair.second->getInfo();
      std::cout << "\n";
    }
  }

  void createGroup(int groupId) {
    if (groups.count(groupId)) {
      std::cout << "\ngroup with id " << groupId << " already exists\n\n";
      return;
    }

    auto group = std::make_shared<Group>(groupId);
    groups.insert(std::make_pair(groupId, group));

    std::cout << "Group with id " << groupId << " was successfully created\n\n";
  }

  void getGroup(int groupId) {
    findGroup(groupId)->getInfo();
    std::cout << "\n";
  }

  std::shared_ptr<Group> findGroup(int groupId) {
    auto group = groups.find(groupId);
    if (group == groups.end()) {
      std::cout << "\ngroup with id " << groupId << " doesn't exists\n\n";
      return nullptr;
    }
    return group->second;
  }

  void deleteGroup(int groupId) {
    auto group = findGroup(groupId);

    if (group) {
      groups.erase(groupId);
      std::cout << "Group with id " << groupId
                << " was successfully deleted\n\n";
    }
  }

  void allGroups() const {
    for (const auto &group_pair : groups) {
      group_pair.second->getInfo();
    }
  }

private:
  std::map<int, std::shared_ptr<User>> users;
  std::map<int, std::shared_ptr<Group>> groups;
};

int main() {
  UserManager userManager;

  std::cout << "\nList of allowed commands:\n";
  std::cout << "createUser <userId> <userName> <description> [ <groupId> ]\n";
  std::cout << "deleteUser <userId>\n";
  std::cout << "allUsers\n";
  std::cout << "getUser <userId>\n";
  std::cout << "createGroup <groupId>\n";
  std::cout << "deleteGroup <groupId>\n";
  std::cout << "allGroups\n";
  std::cout << "getGroup <groupId>\n";
  std::cout << "help\n";
  std::cout << "exit\n";
  std::cout << "\nEnter your choice: \n";

  while (true) {
    std::string choice;
    std::getline(std::cin, choice);

    std::vector<std::string> command;
    std::stringstream ss(choice);

    for (std::string token; ss >> token;) {
      command.push_back(token);
    }

    if (command.size() == 0) {
      std::cout << "\nplease enter one of the commands\n" << std::endl;
      continue;
    }

    if (command[0] == "createUser") {
      try {
        int userId = std::stoi(command.at(1));
        std::string userName = command.at(2);
        std::string description = command.at(3);
        if (command.size() >= 5) {
          int groupId = std::stoi(command.at(4));
          userManager.createUser(userId, userName, description, groupId);
        } else {
          userManager.createUser(userId, userName, description);
        }
      } catch (const std::out_of_range &) {
        std::cout << "\nnot enough arguments\n\n" << std::endl;
      } catch (const std::invalid_argument &) {
        std::cout << command.at(1) << "\ninvalid argument type\n\n"
                  << std::endl;
      }
    }

    else if (command[0] == "allUsers") {
      userManager.allUsers();
    }

    else if (command[0] == "getUser") {
      try {
        int userId = std::stoi(command.at(1));
        userManager.getUser(userId);
      } catch (const std::out_of_range &) {
        std::cout << "\nnot enough arguments\n\n" << std::endl;
      } catch (const std::invalid_argument &) {
        std::cout << command.at(1) << "\ninvalid argument type\n\n"
                  << std::endl;
      }
    }

    else if (command[0] == "deleteUser") {
      try {
        int userId = std::stoi(command.at(1));
        userManager.deleteUser(userId);
      } catch (const std::out_of_range &) {
        std::cout << "\nnot enough arguments\n\n" << std::endl;
      } catch (const std::invalid_argument &) {
        std::cout << command.at(1) << "\ninvalid argument type\n\n"
                  << std::endl;
      }
    }

    else if (command[0] == "allGroups") {
      userManager.allGroups();
    }

    else if (command[0] == "createGroup") {
      try {
        int groupId = std::stoi(command.at(1));
        userManager.createGroup(groupId);
      } catch (const std::out_of_range &) {
        std::cout << "\nnot enough arguments\n\n" << std::endl;
      } catch (const std::invalid_argument &) {
        std::cout << command.at(1) << "\ninvalid argument type\n\n"
                  << std::endl;
      }
    }

    else if (command[0] == "getGroup") {
      try {
        int groupId = std::stoi(command.at(1));
        userManager.getGroup(groupId);
      } catch (const std::out_of_range &) {
        std::cout << "\nnot enough arguments\n\n" << std::endl;
      } catch (const std::invalid_argument &) {
        std::cout << command.at(1) << "\ninvalid argument type\n\n"
                  << std::endl;
      }
    }

    else if (command[0] == "deleteGroup") {
      try {
        int groupId = std::stoi(command.at(1));
        userManager.deleteGroup(groupId);
      } catch (const std::out_of_range &) {
        std::cout << "\nnot enough arguments\n\n" << std::endl;
      } catch (const std::invalid_argument &) {
        std::cout << command.at(1) << "\ninvalid argument type\n\n"
                  << std::endl;
      }
    }

    else if (choice == "help") {
      std::cout << "\n\n\nList of allowed commands:\n";
      std::cout << "createUser <userId> <userName> <description>\n";
      std::cout << "deleteUser <userId>\n";
      std::cout << "allUsers\n";
      std::cout << "getUser <userId>\n";
      std::cout << "createGroup <groupId>\n";
      std::cout << "deleteGroup <groupId>\n";
      std::cout << "allGroups\n";
      std::cout << "getGroup <groupId>\n";
      std::cout << "help\n";
      std::cout << "exit\n";
      std::cout << "\nEnter your choice: \n\n";
    } else if (choice == "exit") {
      return 0;
    }

    else {
      std::cout
          << "\ntype command >>help to see the list of allowed commands\n\n";
    }
  }

  return 0;
}
