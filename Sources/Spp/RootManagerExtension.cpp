/**
 * @file Spp/RootManagerExtension.cpp
 * Contains the implementation of class Spp::RootManagerExtension.
 *
 * @copyright Copyright (C) 2026 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/license.html>.
 */
//==============================================================================

#include "spp.h"

namespace Spp
{

//==============================================================================
// Initialization Functions

RootManagerExtension::Overrides* RootManagerExtension::extend(
  Core::Main::RootManager *rootManager,
  SharedPtr<BuildManager> const &buildManager,
  SharedPtr<CodeGen::AstProcessor> const &astProcessor,
  SharedPtr<Rt::GrammarMgr> const &grammarM,
  SharedPtr<Rt::AstMgr> const &astM,
  SharedPtr<Rt::BuildMgr> const &buildM
) {
  auto extension = newSrdObj<RootManagerExtension>(rootManager);
  rootManager->addDynamicInterface(extension);

  auto overrides = new Overrides();
  extension->buildManager = buildManager;
  extension->astProcessor = astProcessor;
  extension->rtGrammarMgr = grammarM;
  extension->rtAstMgr = astM;
  extension->rtBuildMgr = buildM;

  overrides->importFileRef = extension->importFile.set(&RootManagerExtension::_importFile).get();
  overrides->prefixAlususTemplateClassFuncExpNamesRef = extension->prefixAlususTemplateClassFuncExpNames.set(
    &RootManagerExtension::_prefixAlususTemplateClassFuncExpNames
  ).get();

  return overrides;
}


void RootManagerExtension::unextend(Core::Main::RootManager *rootManager, Overrides *overrides)
{
  auto extension = ti_cast<RootManagerExtension>(rootManager);
  extension->importFile.reset(overrides->importFileRef);
  extension->prefixAlususTemplateClassFuncExpNames.reset(overrides->prefixAlususTemplateClassFuncExpNamesRef);
  extension->buildManager.remove();
  extension->astProcessor.remove();
  extension->rtGrammarMgr.remove();
  extension->rtAstMgr.remove();
  extension->rtBuildMgr.remove();
  rootManager->removeDynamicInterface<RootManagerExtension>();
  delete overrides;
}


//==============================================================================
// Main Functions

void RootManagerExtension::_importFile(TiObject *self, Char const *filename)
{
  PREPARE_SELF(rootManager, Core::Main::RootManager);
  Str error;
  if (!rootManager->tryImportFile(filename, error)) {
    throw EXCEPTION(FileException, filename, C('r'), error);
  }
}


void RootManagerExtension::_prefixAlususTemplateClassFuncExpNames(
  TiObject *self, Core::Ast::Node *classAst, Core::Ast::Node *argAst
) {
  PREPARE_SELF(rootManager, Core::Main::RootManager);
  auto rootManagerExt = ti_cast<RootManagerExtension>(rootManager);

  auto astHelper = rootManagerExt->rtAstMgr->getAstHelper();
  Str prefix;
  Core::Ast::Node *node = astHelper->traceType(argAst);
  while (node->getOwner() != 0) {
    if (node->getOwner()->isDerivedFrom<Core::Ast::Definition>()) {
      prefix = static_cast<Core::Ast::Definition*>(node->getOwner())->getName().getStr() + prefix;
    }
    node = node->getOwner();
  }
  prefix += S("_");

  static SharedPtr<Core::Ast::Node> funcMatchRef;
  if (funcMatchRef == 0) {
    funcMatchRef = rootManager->parseExpression(S("elementType == \"function\""));
  }
  astHelper->getSeeker()->extForeach(funcMatchRef.get(), classAst,
    [&prefix](
      TiInt action, Core::Ast::Node *obj, Core::Ast::Seeker::NoticePtr const &notice
    )->Core::Ast::Seeker::Verb {
      if (action != Core::Ast::Seeker::Action::TARGET_MATCH) return Core::Ast::Seeker::Verb::MOVE;
      auto func = ti_cast<Ast::Function>(obj);
      // Only functions with an explicit exported name are prefixed. Other functions have no name of their own,
      // so prefixing them would make the different instances of the template collide with each other.
      if (func != 0 && func->getName().getStr().getLength() > 0) {
        if (Srl::String::compare(func->getName().get(), prefix.getBuf(), prefix.getLength()) != 0) {
          func->setName((prefix + func->getName().get()).getBuf());
        }
      }
      return Core::Ast::Seeker::Verb::MOVE;
    },
    Core::Ast::Seeker::Flags::SKIP_OWNERS |
    Core::Ast::Seeker::Flags::SKIP_USES |
    SeekerExtension::Flags::SKIP_CHILDREN
  );
}

} // namespace
