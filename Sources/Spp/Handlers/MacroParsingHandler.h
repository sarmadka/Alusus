/**
 * @file Spp/Handlers/MacroParsingHandler.h
 *
 * @copyright Copyright (C) 2019 Sarmad Khalid Abdullah
 *
 * @license This file is released under Alusus Public License, Version 1.0.
 * For details on usage and copying conditions read the full license in the
 * accompanying license file or at <https://alusus.org/alusus_license_1_0>.
 */
//==============================================================================

#ifndef SPP_HANDLERS_MACROPARSINGHANDLER_H
#define SPP_HANDLERS_MACROPARSINGHANDLER_H

namespace Spp::Handlers
{

class MacroParsingHandler : public Core::Processing::Handlers::GenericParsingHandler
{
  //============================================================================
  // Type Info

  TYPE_INFO(MacroParsingHandler, Core::Processing::Handlers::GenericParsingHandler,
            "Spp.Handlers", "Spp", "alusus.org");


  //============================================================================
  // Constructor

  public: MacroParsingHandler()
  {
  }


  //============================================================================
  // Member Functions

  public: virtual void onProdEnd(Core::Processing::Parser *parser, Core::Processing::ParserState *state);

  private: Bool parseArgs(
    Core::Processing::ParserState *state, Core::Data::Ast::Bracket *bracket, SharedPtr<Core::Data::Ast::Map> &result
  );

  private: Bool parseArg(
    Core::Processing::ParserState *state, TiObject *arg, SharedPtr<Core::Data::Ast::Map> const &result
  );

  public: virtual Bool onIncomingModifier(
    Core::Processing::Parser *parser, Core::Processing::ParserState *state,
    TioSharedPtr const &modifierData, Bool prodProcessingComplete
  );

}; // class

} // namespace

#endif
