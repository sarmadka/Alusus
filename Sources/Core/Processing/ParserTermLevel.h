/**
 * @file Core/Processing/ParserTermLevel.h
 * Contains the header of class Core::Processing::ParserTermLevel.
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef CORE_PROCESSING_PARSERTERMLEVEL_H
#define CORE_PROCESSING_PARSERTERMLEVEL_H

namespace Core { namespace Processing
{

/**
 * @brief Defines one level in the state's hierarchy position stack.
 * @ingroup core_processing
 *
 * The ParserState object has a stack that defines the current parsing position within
 * the terms hierarchy. Each level in this stack is represented by a single
 * instance of this structure.
 */
class ParserTermLevel
{
  //============================================================================
  // Friend Classes

  friend class ParserState;


  //============================================================================
  // Member Variables

  /**
   * @brief Defines where at this level the state machine is standing.
   *
   * The meaning of the number returned depends on the term using it.
   */
  private: Word posId;

  /// Pointer to the current level's term object.
  private: Data::Grammar::Term *term;

  /// @sa setParam1()
  private: Data::PlainPairedPtr param1;

  /// @sa setParam2()
  private: Data::PlainPairedPtr param2;

  /// @sa setParam3()
  private: Data::PlainPairedPtr param3;

  /// @sa setFlags()
  private: Data::PlainPairedPtr flags;


  //============================================================================
  // Constructors / Destructor

  /// Initialize all members to 0.
  public: ParserTermLevel() : posId(0), term(0)
  {
  }

  /// Copy the state level and increment data users count.
  public: ParserTermLevel(const ParserTermLevel &level) : posId(level.getPosId()),
    term(level.getTerm()),
    param1(*(level.getParam1())),
    param2(*(level.getParam2())),
    param3(*(level.getParam3())),
    flags(*(level.getFlags()))
  {
  }

  public: ~ParserTermLevel()
  {
  }


  //============================================================================
  // Member Functions

  /**
   * @brief Set the identifier of ths current position within this level.
   *
   * The meaning of the number returned depends on the term using it.
   */
  protected: void setPosId(Word i)
  {
    this->posId = i;
  }

  /**
   * @brief Get the identifier of the current position within this level.
   *
   * The meaning of the number returned depends on the term using it.
   */
  public: Word getPosId() const
  {
    return this->posId;
  }

  /// Set the term object of this level.
  protected: void setTerm(Data::Grammar::Term *t)
  {
    this->term = t;
  }

  /// Get the term object of this level.
  public: Data::Grammar::Term* getTerm() const
  {
    return this->term;
  }

  /**
   * @brief Get a first parameter for the level's term.
   * This is used for optimization purposes, to avoid tracing the term's
   * parameters everytime the level is revisited. This is needed mainly for
   * concat terms, alternate terms, as well as multiply terms.
   */
  protected: Data::PlainPairedPtr* getParam1()
  {
    return &this->param1;
  }
  protected: Data::PlainPairedPtr const* getParam1() const
  {
    return &this->param1;
  }

  /**
   * @brief Set a second parameter for the level's term.
   * This is used for optimization purposes, to avoid tracing the term's
   * parameters everytime the level is revisited. This is needed mainly for
   * concat terms, alternate terms, as well as multiply terms.
   */
  protected: Data::PlainPairedPtr* getParam2()
  {
    return &this->param2;
  }
  protected: Data::PlainPairedPtr const* getParam2() const
  {
    return &this->param2;
  }

  /**
   * @brief Set a third parameter for the level's term.
   * This is used for optimization purposes, to avoid tracing the term's
   * parameters everytime the level is revisited. This is needed mainly for
   * concat terms, alternate terms, as well as multiply terms.
   */
  protected: Data::PlainPairedPtr* getParam3()
  {
    return &this->param3;
  }
  protected: Data::PlainPairedPtr const* getParam3() const
  {
    return &this->param3;
  }

  protected: Data::PlainPairedPtr* getFlags()
  {
    return &this->flags;
  }
  protected: Data::PlainPairedPtr const* getFlags() const
  {
    return &this->flags;
  }

  public: void copyFrom(ParserTermLevel *src)
  {
    this->setTerm(src->getTerm());
    this->setPosId(src->getPosId());
    this->param1 = *src->getParam1();
    this->param2 = *src->getParam2();
    this->param3 = *src->getParam3();
    this->flags = *src->getFlags();
  }

}; // class

} } // namespace

#endif
