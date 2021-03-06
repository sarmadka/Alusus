/**
 * @file Core/Data/Ast/ParamPass.cpp
 * Contains the implementation of class Core::Data::Ast::ParamPass.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core { namespace Data { namespace Ast
{

//==============================================================================
// Clonable Implementation

SharedPtr<TiObject> ParamPass::clone() const
{
  SharedPtr<ParamPass> newParamPass = std::make_shared<ParamPass>();
  newParamPass->setProdId(this->getProdId());
  for (Word i = 0; i < this->getElementCount(); ++i) {
    newParamPass->setElement(i, this->getElement(i));
  }
  newParamPass->setType(this->type);
  newParamPass->setSourceLocation(this->getSourceLocation());
  return newParamPass;
}

} } } // namespace
