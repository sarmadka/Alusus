/**
 * @file Core/Data/Grammar/grammar.cpp
 * Contains the global implementations of Data::Grammar namespace's declarations.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#include "core.h"

namespace Core::Data::Grammar
{

SharedPtr<Reference> createReference(Char const *qualifier, std::vector<SharedPtr<Reference>> *referenceCache)
{
  VALIDATE_NOT_NULL(qualifier);

  SharedPtr<Reference> ref, prevRef;

  Int cacheIndex = 0;
  auto addSegment = [=, &ref, &prevRef, &cacheIndex](Int begin, Word size)
  {
    SharedPtr<Reference> seg;
    if (referenceCache != 0) {
      if (cacheIndex >= referenceCache->size()) {
        referenceCache->push_back(std::make_shared<Reference>());
      }
      seg = referenceCache->at(cacheIndex);
      ++cacheIndex;
    } else {
      seg = std::make_shared<Reference>(qualifier + begin, size);
    }
    seg->setKey(qualifier + begin, size);
    seg->setNext(SharedPtr<Reference>::null);
    if (prevRef != 0) {
      prevRef->setNext(seg);
    }
    prevRef = seg;
    if (ref == 0) {
      ref = seg;
    }
  };

  Int start = 0, count = 0;
  while ((count = SBSTR(qualifier + start).findPos('.')) >= 0) {
    addSegment(start, count);
    start += count + 1;
  }
  addSegment(start, SBSTR(qualifier).size() - start);
  ASSERT(ref != 0);

  return ref;
}


Module* getGrammarRoot(DynamicContaining<TiObject> *rootScope, Bool createIfMissing)
{
  VALIDATE_NOT_NULL(rootScope);

  // Find grammar root.
  Module *grammarRoot = 0;
  for (Int i = 0; i < rootScope->getElementCount(); ++i) {
    auto def = ti_cast<Data::Ast::Definition>(rootScope->getElement(i));
    if (def != 0 && def->getName() == S("grammar")) {
      grammarRoot = def->getTarget().ti_cast_get<Module>();
      break;
    }
  }

  // Create a new grammar root if no one is found.
  if (grammarRoot == 0 && createIfMissing) {
    auto grammarDef = Data::Ast::Definition::create();
    grammarDef->setName(S("grammar"));
    grammarDef->setTarget(Module::create({}));
    rootScope->addElement(grammarDef.get());
    grammarRoot = grammarDef->getTarget().s_cast_get<Module>();
  }

  return grammarRoot;
}


Bool matchCharGroup(WChar ch, CharGroupUnit *unit)
{
  ASSERT(unit);

  if (unit->isA<SequenceCharGroupUnit>()) {
    SequenceCharGroupUnit *u = static_cast<SequenceCharGroupUnit*>(unit);
    if (u->getStartCode() == 0 && u->getEndCode() == 0) {
      throw EXCEPTION(GenericException, S("Sequence char group unit is not configured yet."));
    }
    if (ch >= u->getStartCode() && ch <= u->getEndCode()) return true;
    else return false;
  } else if (unit->isA<RandomCharGroupUnit>()) {
    RandomCharGroupUnit *u = static_cast<RandomCharGroupUnit*>(unit);
    if (u->getCharList() == 0) {
      throw EXCEPTION(GenericException, S("Random char group unit is not configured yet."));
    }
    for (Int i = 0; i < u->getCharListSize(); i++) {
      if (u->getCharList()[i] == ch) return true;
    }
    return false;
  } else if (unit->isA<UnionCharGroupUnit>()) {
    UnionCharGroupUnit *u = static_cast<UnionCharGroupUnit*>(unit);
    if (u->getCharGroupUnits()->size() == 0) {
      throw EXCEPTION(GenericException, S("Union char group unit is not configured yet."));
    }
    for (Int i = 0; i < static_cast<Int>(u->getCharGroupUnits()->size()); i++) {
      if (matchCharGroup(ch, u->getCharGroupUnits()->at(i).get()) == true) {
        return true;
      }
    }
    return false;
  } else if (unit->isA<InvertCharGroupUnit>()) {
    InvertCharGroupUnit *u = static_cast<InvertCharGroupUnit*>(unit);
    if (u->getChildCharGroupUnit() == 0) {
      throw EXCEPTION(GenericException, S("Invert char group unit is not configured yet."));
    }
    if (matchCharGroup(ch, u->getChildCharGroupUnit().get()) == true) {
      return false;
    } else {
      return true;
    }
  } else {
    throw EXCEPTION(GenericException, S("Invalid char group type."));
  }
  return false; // just to prevent warnings
}


void setTreeIds(TiObject *obj)
{
  StrStream stream;
  Node *node = ti_cast<Node>(obj);
  if (node != 0) generateId(node, stream);
  setTreeIds(obj, stream.str().c_str());
}


void setTreeIds(TiObject *obj, const Char *id)
{
  IdHaving *idh = ti_cast<IdHaving>(obj);
  if (idh != 0) idh->setId(ID_GENERATOR->getId(id));

  StrStream childId;
  MapContaining<TiObject> *map; Containing<TiObject> *list;
  if ((map = ti_cast<MapContaining<TiObject>>(obj)) != 0) {
    for (Int i = 0; static_cast<Word>(i) < map->getElementCount(); ++i) {
      childId.str(Str());
      childId << id;
      if (childId.tellp() != 0) childId << C('.');
      childId << map->getElementKey(i).c_str();
      setTreeIds(map->getElement(i), childId.str().c_str());
    }
  } else if ((list = ti_cast<Containing<TiObject>>(obj)) != 0) {
    for (Int i = 0; static_cast<Word>(i) < list->getElementCount(); ++i) {
      childId.str(Str());
      childId << id;
      if (childId.tellp() != 0) childId << C('.');
      childId << i;
      setTreeIds(list->getElement(i), childId.str().c_str());
    }
  }
}


void generateId(Node *obj, StrStream &id)
{
  if (obj == 0) {
    throw EXCEPTION(InvalidArgumentException, S("obj"), S("Value is null."));
  }
  auto owner = ti_cast<Module>(obj->getOwner());
  if (owner == 0) return;
  generateId(owner, id);
  if (id.tellp() != 0) id << C('.');
  for (Int i = 0; i < owner->getElementCount(); ++i) {
    if (owner->getElement(i) == obj) {
      auto mapContainer = owner->getInterface<MapContaining<TiObject>>();
      if (mapContainer != 0) {
        id << mapContainer->getElementKey(i).c_str();
      } else {
        id << i;
      }
      return;
    }
  }
  throw EXCEPTION(GenericException, S("The provided object has an invalid owner."));
}

} // namespace
