/**
 * @file Core/Ast/Node.h
 * Contains the definitions of Core::Ast::Node.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#ifndef CORE_AST_NODE_H
#define CORE_AST_NODE_H

namespace Core::Ast
{

/**
 * @brief The root of all AST classes.
 * @ingroup core_data_ast
 * This class links AST objects to their owners. This is needed to allow
 * moving upwards through AST trees nodes.
 */
class Node : public TiObject
{
  //============================================================================
  // Type Info

  TYPE_INFO(Node, TiObject, "Core.Ast", "Core", "alusus.org");


  //============================================================================
  // Member Variables

  private: Node *owner;


  //============================================================================
  // Constructors

  public: Node() : owner(0)
  {
  }


  //============================================================================
  // Member Functions

  public: void setOwner(Node *o)
  {
    this->owner = o;
  }

  public: Node* getOwner() const
  {
    return this->owner;
  }

  /// Find a node's owner of a specific type.
  template<class T> T* findOwner() const
  {
    Node *node = this->getOwner();
    while (node != 0 && !node->isDerivedFrom<T>()) node = node->getOwner();
    return static_cast<T*>(node);
  }

}; // class

} // namespace

#endif
