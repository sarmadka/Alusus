/**
 * @file Spp/CodeGen/MacroProcessor.cpp
 * Contains the implementation of class Spp::CodeGen::MacroProcessor.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#include "spp.h"

namespace Spp::CodeGen
{

//==============================================================================
// Initialization Functions

void MacroProcessor::initBindingCaches()
{
  Basic::initBindingCaches(this, {
    &this->runMacroPass,
    &this->processMacro,
    &this->processMacros,
    &this->applyMacroArgs,
    &this->applyMacroArgsIteration,
    &this->applyMacroArgsIteration_identifier,
    &this->applyMacroArgsIteration_stringLiteral,
    &this->applyMacroArgsIteration_tiStr,
    &this->applyMacroArgsIteration_clonable,
    &this->applyMacroArgsIteration_binding,
    &this->applyMacroArgsIteration_containing,
    &this->applyMacroArgsIteration_mapContaining
  });
}


void MacroProcessor::initBindings()
{
  this->runMacroPass = &MacroProcessor::_runMacroPass;
  this->processMacro = &MacroProcessor::_processMacro;
  this->processMacros = &MacroProcessor::_processMacros;
  this->applyMacroArgs = &MacroProcessor::_applyMacroArgs;
  this->applyMacroArgsIteration = &MacroProcessor::_applyMacroArgsIteration;
  this->applyMacroArgsIteration_identifier = &MacroProcessor::_applyMacroArgsIteration_identifier;
  this->applyMacroArgsIteration_stringLiteral = &MacroProcessor::_applyMacroArgsIteration_stringLiteral;
  this->applyMacroArgsIteration_tiStr = &MacroProcessor::_applyMacroArgsIteration_tiStr;
  this->applyMacroArgsIteration_clonable = &MacroProcessor::_applyMacroArgsIteration_clonable;
  this->applyMacroArgsIteration_binding = &MacroProcessor::_applyMacroArgsIteration_binding;
  this->applyMacroArgsIteration_containing = &MacroProcessor::_applyMacroArgsIteration_containing;
  this->applyMacroArgsIteration_mapContaining = &MacroProcessor::_applyMacroArgsIteration_mapContaining;
}


//==============================================================================
// Main Functions

void MacroProcessor::preparePass(Core::Notices::Store *noticeStore)
{
  this->setNoticeStore(noticeStore);
  this->getAstHelper()->prepare(noticeStore);
}


Bool MacroProcessor::_runMacroPass(TiObject *self, Core::Data::Ast::Scope *root)
{
  PREPARE_SELF(macroProcessor, MacroProcessor);
  VALIDATE_NOT_NULL(root);

  Bool result = true;
  for (Int i = 0; i < root->getCount(); ++i) {
    auto def = ti_cast<Data::Ast::Definition>(root->getElement(i));
    if (def != 0) {
      auto module = def->getTarget().ti_cast_get<Spp::Ast::Module>();
      if (module != 0) {
        if (!macroProcessor->processMacros(module)) result = false;
      }
    }
  }
  return result;
}


Bool MacroProcessor::_processMacros(TiObject *self, TiObject *owner)
{
  PREPARE_SELF(macroProcessor, MacroProcessor);
  VALIDATE_NOT_NULL(owner);

  auto container = ti_cast<Containing<TiObject>>(owner);
  if (container == 0) return true;

  Bool result = true;
  for (Int i = 0; i < container->getElementCount(); ++i) {
    auto child = container->getElement(i);
    if (child == 0) continue;
    if (child->isDerivedFrom<Core::Data::Ast::ParamPass>()) {
      auto paramPass = static_cast<Core::Data::Ast::ParamPass*>(child);
      if (paramPass->getType() == Core::Data::Ast::BracketType::SQUARE) {
        // Extract args.
        PlainList<TiObject> argsList;
        auto param = paramPass->getParam().get();
        Containing<TiObject> *args = &argsList;
        if (param) {
          if (param->isDerivedFrom<Core::Data::Ast::List>()) {
            args = static_cast<Core::Data::Ast::List*>(param);
          } else {
            argsList.add(param);
          }
        }

        // Find matching macro.
        Ast::Macro *macro = 0;
        macroProcessor->astHelper->getSeeker()->foreach(paramPass->getOperand().get(), owner,
          [=, &macro] (TiObject *obj, Core::Notices::Notice *ntc)->Core::Data::Seeker::Verb
          {
            if (ntc != 0) {
              return Core::Data::Seeker::Verb::MOVE;
            }

            auto m = ti_cast<Ast::Macro>(obj);
            if (m != 0 && m->matchCall(args, macroProcessor->astHelper)) {
              macro = m;
              return Core::Data::Seeker::Verb::STOP;
            } else {
              return Core::Data::Seeker::Verb::MOVE;
            }
          }, 0
        );

        if (macro != 0) {
          auto sl = paramPass->findSourceLocation();
          macroProcessor->noticeStore->pushPrefixSourceLocation(sl.get());
          if (macroProcessor->processMacro(macro, args, owner, i, sl.get())) --i;
          else result = false;
          macroProcessor->noticeStore->popPrefixSourceLocation(
            Core::Data::getSourceLocationRecordCount(sl.get())
          );
        }
        continue;
      }
    }
    if (!macroProcessor->processMacros(child)) result = false;
  }
  return result;
}


Bool MacroProcessor::_processMacro(
  TiObject *self, Spp::Ast::Macro *macro, Containing<TiObject> *args, TiObject *owner, TiInt indexInOwner,
  Core::Data::SourceLocation *sl
) {
  PREPARE_SELF(macroProcessor, MacroProcessor);
  VALIDATE_NOT_NULL(macro, args, owner);

  // Apply macro args.
  TioSharedPtr macroInstance;
  if (!macroProcessor->applyMacroArgs(macro, args, sl, macroInstance)) return false;
  if (macroInstance == 0) {
    macroProcessor->noticeStore->add(
      std::make_shared<Spp::Notices::InvalidMacroNotice>(macro->findSourceLocation())
    );
    return false;
  }
  if (macroInstance->isDerivedFrom<Core::Data::Ast::Scope>()) {
    auto scope = macroInstance.s_cast_get<Core::Data::Ast::Scope>();
    if (scope->getCount() == 1) {
      auto child = scope->get(0);
      macroInstance = child;
    }
  }

  // Replace macro reference with the clone.
  if (owner->isDerivedFrom<Core::Data::Ast::Scope>() && macroInstance->isDerivedFrom<Core::Data::Ast::Scope>()) {
    // Merge the two scopes.
    auto ownerScope = static_cast<Core::Data::Ast::Scope*>(owner);
    auto instanceScope = macroInstance.s_cast_get<Core::Data::Ast::Scope>();
    ownerScope->remove(indexInOwner);
    for (Int i = 0; i < instanceScope->getCount(); ++i) {
      ownerScope->insert(indexInOwner + i, instanceScope->get(i));
    }
  } else {
    // Replace the instance.
    auto ownerContainer = ti_cast<Containing<TiObject>>(owner);
    if (ownerContainer == 0) {
      throw EXCEPTION(InvalidArgumentException, S("owner"), S("Invalid owner type."));
    }
    ownerContainer->setElement(indexInOwner, macroInstance.get());
  }

  return true;
}


Bool MacroProcessor::_applyMacroArgs(
  TiObject *self, Spp::Ast::Macro *macro, Containing<TiObject> *args, Core::Data::SourceLocation *sl,
  TioSharedPtr &result
) {
  PREPARE_SELF(macroProcessor, MacroProcessor);
  VALIDATE_NOT_NULL(macro, args);

  if (macro->getArgCount() != args->getElementCount()) {
    throw EXCEPTION(GenericException, S("Invalid args passed to macro."));
  }

  return macroProcessor->applyMacroArgsIteration(macro->getBody().get(), macro->getArgTypes().get(), args, sl, result);
}


Bool MacroProcessor::_applyMacroArgsIteration(
  TiObject *self, TiObject *obj, Core::Data::Ast::Map *argTypes, Containing<TiObject> *args,
  Core::Data::SourceLocation *sl, TioSharedPtr &result
) {
  PREPARE_SELF(macroProcessor, MacroProcessor);

  if (obj->isDerivedFrom<Core::Data::Ast::Identifier>()) {
    auto identifier = static_cast<Core::Data::Ast::Identifier*>(obj);
    return macroProcessor->applyMacroArgsIteration_identifier(identifier, argTypes, args, sl, result);
  }

  if (obj->isDerivedFrom<Core::Data::Ast::StringLiteral>()) {
    auto stringLiteral = static_cast<Core::Data::Ast::StringLiteral*>(obj);
    return macroProcessor->applyMacroArgsIteration_stringLiteral(stringLiteral, argTypes, args, sl, result);
  }

  if (obj->isDerivedFrom<TiStr>()) {
    auto str = static_cast<TiStr*>(obj);
    return macroProcessor->applyMacroArgsIteration_tiStr(str, argTypes, args, sl, result);
  }

  // It's not a replacable identifier, so we'll proceed with cloning the tree.
  auto clonable = ti_cast<Core::Data::Clonable>(obj);
  if (clonable != 0) {
    return macroProcessor->applyMacroArgsIteration_clonable(clonable, argTypes, args, sl, result);
  }

  result.reset();
  return true;
}


Bool MacroProcessor::_applyMacroArgsIteration_identifier(
  TiObject *self, Core::Data::Ast::Identifier *obj, Core::Data::Ast::Map *argTypes, Containing<TiObject> *args,
  Core::Data::SourceLocation *sl, TioSharedPtr &result
) {
  PREPARE_SELF(macroProcessor, MacroProcessor);
  Char var[1000];
  Word prefixSize = 0;
  Char const *suffix = 0;
  macroProcessor->parseStringTemplate(obj->getValue().get(), var, 1000, prefixSize, suffix);
  auto index = argTypes->findIndex(var);
  if (index != -1) {
    auto arg = args->getElement(index);
    if (prefixSize != 0 || suffix != 0) {
      // We have an identifier string template, so we need the matching arg to be an identifier as well.
      if (arg->isDerivedFrom<Core::Data::Ast::Identifier>()) {
        Char newVar[1000];
        macroProcessor->generateStringFromTemplate(
          obj->getValue().get(), prefixSize, static_cast<Core::Data::Ast::Identifier*>(arg)->getValue().get(), suffix,
          newVar, 1000
        );
        result = Core::Data::Ast::Identifier::create({
          { S("value"), TiStr(newVar) },
          { S("sourceLocation"), obj->getSourceLocation() }
        });
        macroProcessor->addSourceLocation(result.get(), sl);
      } else {
        macroProcessor->noticeStore->add(
          std::make_shared<Spp::Notices::InvalidMacroArgNotice>(Core::Data::Ast::findSourceLocation(arg))
        );
        return false;
      }
    } else {
      // We don't have an identifier string template, so we'll just copy the arg as is.
      result = macroProcessor->cloneTree(args->getElement(index), sl);
    }
  } else {
    result = obj->clone();
    macroProcessor->addSourceLocation(result.get(), sl);
  }
  return true;
}


Bool MacroProcessor::_applyMacroArgsIteration_stringLiteral(
  TiObject *self, Core::Data::Ast::StringLiteral *obj, Core::Data::Ast::Map *argTypes, Containing<TiObject> *args,
  Core::Data::SourceLocation *sl, TioSharedPtr &result
) {
  PREPARE_SELF(macroProcessor, MacroProcessor);
  Char var[1000];
  Word prefixSize = 0;
  Char const *suffix = 0;
  macroProcessor->parseStringTemplate(obj->getValue().get(), var, 1000, prefixSize, suffix, S("{{"), S("}}"));
  auto index = argTypes->findIndex(var);
  if (index != -1) {
    auto arg = args->getElement(index);
    if (prefixSize != 0 || suffix != 0) {
      // We have an identifier string template, so we need the matching arg to be an identifier as well.
      if (arg->isDerivedFrom<Core::Data::Ast::Identifier>() || arg->isDerivedFrom<Core::Data::Ast::StringLiteral>()) {
        Char newVar[1000];
        macroProcessor->generateStringFromTemplate(
          obj->getValue().get(), prefixSize, static_cast<Core::Data::Ast::Text*>(arg)->getValue().get(), suffix,
          newVar, 1000
        );
        result = Core::Data::Ast::StringLiteral::create({
          { S("value"), TiStr(newVar) },
          { S("sourceLocation"), obj->getSourceLocation() }
        });
        macroProcessor->addSourceLocation(result.get(), sl);
      } else {
        macroProcessor->noticeStore->add(
          std::make_shared<Spp::Notices::InvalidMacroArgNotice>(Core::Data::Ast::findSourceLocation(arg))
        );
        return false;
      }
    } else {
      // We don't have an identifier string template, so we'll just copy the arg as is.
      result = macroProcessor->cloneTree(args->getElement(index), sl);
    }
  } else {
    result = obj->clone();
    macroProcessor->addSourceLocation(result.get(), sl);
  }
  return true;
}


Bool MacroProcessor::_applyMacroArgsIteration_tiStr(
  TiObject *self, TiStr *obj, Core::Data::Ast::Map *argTypes, Containing<TiObject> *args,
  Core::Data::SourceLocation *sl, TioSharedPtr &result
) {
  PREPARE_SELF(macroProcessor, MacroProcessor);
  Char var[1000];
  Word prefixSize = 0;
  Char const *suffix = 0;
  macroProcessor->parseStringTemplate(obj->get(), var, 1000, prefixSize, suffix);
  auto index = argTypes->findIndex(var);
  if (index != -1) {
    auto arg = ti_cast<Core::Data::Ast::Identifier>(args->getElement(index));
    if (arg != 0) {
      Char newVar[1000];
      macroProcessor->generateStringFromTemplate(obj->get(), prefixSize, arg->getValue().get(), suffix, newVar, 1000);
      result = TiStr::create(newVar);
    } else {
      macroProcessor->noticeStore->add(
        std::make_shared<Spp::Notices::InvalidMacroArgNotice>(Core::Data::Ast::findSourceLocation(arg))
      );
      return false;
    }
  } else {
    result = TiStr::create(obj->get());
  }
  return true;
}


Bool MacroProcessor::_applyMacroArgsIteration_clonable(
  TiObject *self, Core::Data::Clonable *obj, Core::Data::Ast::Map *argTypes, Containing<TiObject> *args,
  Core::Data::SourceLocation *sl, TioSharedPtr &result
) {
  PREPARE_SELF(macroProcessor, MacroProcessor);

  result = obj->clone();
  macroProcessor->addSourceLocation(result.get(), sl);

  auto binding = result.ti_cast_get<Binding>();
  if (binding != 0) {
    if (!macroProcessor->applyMacroArgsIteration_binding(binding, argTypes, args, sl)) return false;
  }

  auto container = result.ti_cast_get<Containing<TiObject>>();
  if (container != 0) {
    if (!macroProcessor->applyMacroArgsIteration_containing(container, argTypes, args, sl)) return false;
  }

  auto mapContainer = result.ti_cast_get<DynamicMapContaining<TiObject>>();
  if (mapContainer != 0) {
    if (!macroProcessor->applyMacroArgsIteration_mapContaining(mapContainer, argTypes, args, sl)) return false;
  }

  return true;
}


Bool MacroProcessor::_applyMacroArgsIteration_binding(
  TiObject *self, Binding *obj, Core::Data::Ast::Map *argTypes, Containing<TiObject> *args,
  Core::Data::SourceLocation *sl
) {
  PREPARE_SELF(macroProcessor, MacroProcessor);
  for (Int i = 0; i < obj->getMemberCount(); ++i) {
    if (obj->getMemberHoldMode(i) == HoldMode::VALUE && obj->getMemberNeededType(i) == TiStr::getTypeInfo()) {
      // replace string, if possible.
      Char var[1000];
      Word prefixSize = 0;
      Char const *suffix = 0;
      macroProcessor->parseStringTemplate(obj->refMember<TiStr>(i).get(), var, 1000, prefixSize, suffix);
      Int index = argTypes->findElementIndex(var);
      if (index != -1) {
        auto identifier = ti_cast<Core::Data::Ast::Identifier>(args->getElement(index));
        if (identifier == 0) {
          auto elementSl = Core::Data::Ast::findSourceLocation(args->getElement(index));
          macroProcessor->noticeStore->add(
            std::make_shared<Spp::Notices::InvalidMacroArgNotice>(elementSl == 0 ? getSharedPtr(sl) : elementSl)
          );
          return false;
        }
        Char newVar[1000];
        macroProcessor->generateStringFromTemplate(
          obj->refMember<TiStr>(i).get(), prefixSize, identifier->getValue().get(), suffix, newVar, 1000
        );
        obj->refMember<TiStr>(i) = newVar;
      }
    } else if (obj->getMemberHoldMode(i) == HoldMode::SHARED_REF) {
      // Clone the member.
      TioSharedPtr newChild;
      TiObject *child = obj->getMember(i);
      if (child != 0) {
        if (!macroProcessor->applyMacroArgsIteration(child, argTypes, args, sl, newChild)) return false;
        if (newChild != 0) obj->setMember(i, newChild.get());
      }
    }
  }
  return true;
}


Bool MacroProcessor::_applyMacroArgsIteration_containing(
  TiObject *self, Containing<TiObject> *obj, Core::Data::Ast::Map *argTypes, Containing<TiObject> *args,
  Core::Data::SourceLocation *sl
) {
  PREPARE_SELF(macroProcessor, MacroProcessor);
  for (Int i = 0; i < obj->getElementCount(); ++i) {
    if (obj->getElementHoldMode(i) == HoldMode::SHARED_REF) {
      TioSharedPtr newChild;
      TiObject *child = obj->getElement(i);
      if (child != 0) {
        if (!macroProcessor->applyMacroArgsIteration(child, argTypes, args, sl, newChild)) return false;
        if (newChild != 0) obj->setElement(i, newChild.get());
      }
    }
  }
  return true;
}


Bool MacroProcessor::_applyMacroArgsIteration_mapContaining(
  TiObject *self, DynamicMapContaining<TiObject> *obj, Core::Data::Ast::Map *argTypes, Containing<TiObject> *args,
  Core::Data::SourceLocation *sl
) {
  PREPARE_SELF(macroProcessor, MacroProcessor);
  for (Int i = 0; i < obj->getElementCount(); ++i) {
    Char var[1000];
    Word prefixSize = 0;
    Char const *suffix = 0;
    macroProcessor->parseStringTemplate(obj->getElementKey(i).c_str(), var, 1000, prefixSize, suffix);
    auto index = argTypes->findElementIndex(var);
    if (index != -1) {
      // The key can be replaced with the new key.
      auto arg = ti_cast<Core::Data::Ast::Identifier>(args->getElement(index));
      if (arg != 0) {
        Char newVar[1000];
        macroProcessor->generateStringFromTemplate(
          obj->getElementKey(i).c_str(), prefixSize, arg->getValue().get(), suffix, newVar, 1000
        );
        // Make sure the new key isn't already used.
        if (obj->findElementIndex(newVar) != -1) {
          macroProcessor->noticeStore->add(
            std::make_shared<Spp::Notices::InvalidMacroArgNotice>(arg->findSourceLocation())
          );
          return false;
        }
        auto value = obj->getElement(index);
        obj->insertElement(index, newVar, value);
        obj->removeElement(index + 1);
      } else {
        macroProcessor->noticeStore->add(
          std::make_shared<Spp::Notices::InvalidMacroArgNotice>(Core::Data::Ast::findSourceLocation(arg))
        );
        return false;
      }
    }
  }
  return true;
}


//==============================================================================
// Helper Functions

TioSharedPtr MacroProcessor::cloneTree(TiObject *obj, Core::Data::SourceLocation *sl)
{
  auto clonable = ti_cast<Core::Data::Clonable>(obj);
  if (clonable == 0) {
    throw EXCEPTION(InvalidArgumentException, S("obj"), S("Must be an object that implements Clonable interface."));
  }

  auto clone = clonable->clone();
  this->addSourceLocation(clone.get(), sl);

  auto container = clone.ti_cast_get<Containing<TiObject>>();
  if (container) {
    for (Int i = 0; i < container->getElementCount(); ++i) {
      auto childClone = this->cloneTree(container->getElement(i), sl);
      container->setElement(i, childClone.get());
    }
  }

  return clone;
}


void MacroProcessor::parseStringTemplate(
  Char const *str, Char *var, Word varBufSize, Word &prefixSize, Char const *&suffix,
  Char const *varOpening, Char const *varClosing
) {
  auto varStart = SBSTR(str).findPos(varOpening) + 2;
  if (varStart >= 2) {
    auto varSize = SBSTR(str + varStart).findPos(varClosing);
    if (varSize > 0) {
      SBSTR(var).assign(str + varStart, varSize, varBufSize);
      prefixSize = varStart - 2;
      suffix = str + varStart + varSize + 2;
      return;
    }
  }
  SBSTR(var).assign(str, varBufSize);
  prefixSize = 0;
  suffix = 0;
}


void MacroProcessor::generateStringFromTemplate(
  Char const *prefix, Word prefixSize, Char const *var, Char const *suffix, Char *output, Word outputBufSize
) {
  SbStr &result = SBSTR(output);
  result.assign(S(""), outputBufSize);
  if (prefixSize > 0) result.append(prefix, prefixSize, outputBufSize);
  result.append(var, outputBufSize);
  if (suffix != 0) result.append(suffix, outputBufSize);
}


void MacroProcessor::addSourceLocation(TiObject *obj, Core::Data::SourceLocation *sl)
{
  auto metadata = ti_cast<Core::Data::Ast::MetaHaving>(obj);
  if (metadata == 0) return;

  auto currentSl = metadata->findSourceLocation();
  if (currentSl == 0) {
    metadata->setSourceLocation(sl);
  } else {
    auto newSl = std::make_shared<Core::Data::SourceLocationStack>();
    newSl->push(sl);
    newSl->push(currentSl.get());
    metadata->setSourceLocation(newSl);
  }
}

} // namespace
