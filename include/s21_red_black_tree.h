#ifndef SRC_CONTAINERS_S21_RED_BLACK_TREE_H_
#define SRC_CONTAINERS_S21_RED_BLACK_TREE_H_

#include <iostream>

namespace s21 {

enum class Color {
  kRed,
  kBlack,
};

template <typename K, typename V>
class RedBlackTree {
 public:
  RedBlackTree() { root_ = nullptr; };
  RedBlackTree(K key, V value) { root_ = new Node(key, value); };
  ~RedBlackTree() { delete root_; };

  void insert(K key, V value) {
    Node *n_node = new Node(key, value);
    if (!root_) {
      root_ = n_node;
      root_->color_ = Color::kBlack;
      return;
    }
    Node *deep = root_;
    while (deep != nullptr) {
      if (deep->key_ <= key) {
        if (deep->right_ == nullptr) {
          n_node->parent_ = deep;
          deep->right_ = n_node;
          break;
        }
        deep = deep->right_;
      } else {
        if (deep->left_ == nullptr) {
          n_node->parent_ = deep;
          deep->left_ = n_node;
          break;
        }
        deep = deep->left_;
      }
    }
    if (!n_node->Granny()) return;
    n_node->AfterInsertFix(this);
    root_->color_ = Color::kBlack;
  };
  void remove(K key) {
    Node *deep = root_;
    while (deep != nullptr && deep->key_ != key) {
      if (deep->key_ <= key) {
        deep = deep->right_;
      } else {
        deep = deep->left_;
      }
    }
    if (!deep) {
      // НЕТ НИКАКОГО EXCEPTION НА СЛУЧАЙ НЕСУЩЕСТВУЮЩЕГО КЛЮЧА
      // а надо ли...
      return;
    }
    if (!deep->right_) {
      deep->SwapAndDelete(deep->left_, this);
    } else if (!deep->left_) {
      deep->SwapAndDelete(deep->right_, this);
    } else {
      // заменить удаляемое на число, которое max из чисел меньше удаляемого
      // или min из чисел больше удаляемого
      Node *max = deep->left_->Max();
      // max->parent_->GetNodeInfo();
      if (max->parent_->left_ == max) {
        max->parent_->left_ = nullptr;
      } else {
        max->parent_->right_ = nullptr;
      }
      // max->parent_->GetNodeInfo();
      deep->SwapAndDelete(max, this);
    }
    // AfterDeleteFix();
  };

  void print() {
    if (root_) {
      root_->PrintHelper("", true, 0);
    }
  };

  //  private:
  class Node {
   public:
    Node(K key, V value) : key_(key), value_(value){};
    Node(Node *parent, K key, V value)
        : parent_(parent), key_(key), value_(value){};
    ~Node() {
      delete left_;
      delete right_;
    };

    Node *Granny() {
      if (parent_) {
        return parent_->parent_;
      }
      return nullptr;
    };
    Node *Uncle() {
      Node *granny = Granny();
      if (!granny) {
        return nullptr;
      }
      if (parent_ == granny->right_) {
        return granny->left_;
      }
      return granny->right_;
    };
    Node *Brother() {
      if (!parent_) {
        return nullptr;
      } else if (parent_->right_ = this) {
        return parent_->left_;
      }
      return parent_->right_;
    };
    Node *Max() {
      Node *maximum = this;
      while (maximum->right_) {
        maximum = maximum->right_;
      }
      return maximum;
    };

    void GetNodeInfo() {
      std::cout << "~~~~~~~~~~~~~~~~~~\n";
      std::cout << "Key: " << key_ << "; Val: " << value_ << std::endl;
      std::cout << "P: " << (parent_ ? parent_->key_ : -1)
                << "; L: " << (left_ ? left_->key_ : -1)
                << "; R: " << (right_ ? right_->key_ : -1) << std::endl;
      std::cout << "~~~~~~~~~~~~~~~~~~\n";
    }

    void RotationLeft(RedBlackTree *tree) {
      Node *replace = right_;
      replace->parent_ = parent_;
      if (!replace->parent_) {
        tree->root_ = replace;
      } else if (parent_->left_ == this) {
        parent_->left_ = replace;
      } else {
        parent_->right_ = replace;
      }
      right_ = replace->left_;
      if (right_) {
        right_->parent_ = this;
      }
      parent_ = replace;
      replace->left_ = this;
    };
    void RotationRight(RedBlackTree *tree) {
      Node *replace = left_;
      replace->parent_ = parent_;
      if (!replace->parent_) {
        tree->root_ = replace;
      } else if (parent_->left_ == this) {
        parent_->left_ = replace;
      } else {
        parent_->right_ = replace;
      }
      left_ = replace->right_;
      if (left_) {
        left_->parent_ = this;
      }
      parent_ = replace;
      replace->right_ = this;
    };

    void SwapAndDelete(Node *child, RedBlackTree *tree) {
      if (!parent_) {
        tree->root_ = child;
      } else if (parent_->left_ == this) {
        parent_->left_ = child;
      } else {
        parent_->right_ = child;
      }
      if (child) {
        child->parent_ = parent_;
      }
      if (left_ && left_ != child) {
        child->left_ = left_;
        left_->parent_ = child;
      }
      if (right_ && right_ != child) {
        child->right_ = right_;
        right_->parent_ = child;
      }
      parent_ = left_ = right_ = nullptr;
      delete this;
    };

    void PrintHelper(std::string indent, bool last, int depth) {
      if (depth < 10) {
        std::cout << indent;
        if (last) {
          std::cout << "R----";
          indent += "     ";
        } else {
          std::cout << "L----";
          indent += "|    ";
        }
        std::string s_color = color_ == Color::kRed ? "R" : "B";
        std::cout << "(" << key_ << ")"
                  << "(" << s_color << ")";
        // std::cout << "(" << (left_ ? left_->key_ : 0) << ", "
        //           << (right_ ? right_->key_ : 0) << ", "
        //           << (parent_ ? parent_->key_ : 0) << ")";
        std::cout << std::endl;
        if (left_ != nullptr) left_->PrintHelper(indent, false, depth + 1);
        if (right_ != nullptr) right_->PrintHelper(indent, true, depth + 1);
      }
    };

    void AfterInsertFix(RedBlackTree *tree) {
      while (parent_ &&
             parent_->color_ == Color::kRed) {  // It's wrong to be commy
        Node *uncle = Uncle();
        if (uncle && uncle->color_ == Color::kRed) {
          FixCaseUncleIsCommy(tree);
        } else {
          FixCaseUncleIsBlack(tree);
        }
      }
    };
    void FixCaseUncleIsCommy(RedBlackTree *tree) {
      Node *uncle = Uncle();
      parent_->color_ = Color::kBlack;
      uncle->color_ = Color::kBlack;
      Node *granny = Granny();
      granny->color_ = Color::kRed;
      granny->AfterInsertFix(tree);
    };
    void FixCaseUncleIsBlack(RedBlackTree *tree) {
      Node *granny = Granny();
      if (granny && this == parent_->right_ && parent_ == granny->left_) {
        parent_->RotationLeft(tree);
        FixCaseRollingUncleAgain(tree);
        return;
      } else if (granny && this == parent_->left_ &&
                 parent_ == granny->right_) {
        parent_->RotationRight(tree);
        FixCaseRollingUncleAgain(tree);
        return;
      }
      parent_->FixCaseRollingUncleAgain(tree);
    };
    void FixCaseRollingUncleAgain(RedBlackTree *tree) {
      color_ = Color::kBlack;
      parent_->color_ = Color::kRed;
      if (this == parent_->left_) {
        parent_->RotationRight(tree);
      } else {
        parent_->RotationLeft(tree);
      }
    };

    void DeleteLeaf() {}

    K key_;
    V value_;
    Node *parent_ = nullptr, *left_ = nullptr, *right_ = nullptr;
    Color color_ = Color::kRed;
  };

  Node *root_;
};
}  // namespace s21

#endif  // SRC_CONTAINERS_S21_RED_BLACK_TREE_H_