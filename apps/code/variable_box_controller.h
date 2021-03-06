#ifndef CODE_VARIABLE_BOX_CONTROLLER_H
#define CODE_VARIABLE_BOX_CONTROLLER_H

#include <escher.h>
#include "script_node.h"
#include "script_node_cell.h"
#include "script_store.h"

namespace Code {

class VariableBoxController : public NestedMenuController {
public:
  VariableBoxController(ScriptStore * scriptStore);

  /* Responder */
  bool handleEvent(Ion::Events::Event event) override;
  void didEnterResponderChain(Responder * previousFirstResponder) override;

  /* ListViewDataSource */
  int numberOfRows() const override;
  int reusableCellCount(int type) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  int typeAtLocation(int i, int j) override;

  /* VariableBoxController */
  void loadFunctionsAndVariables();
private:
  constexpr static int k_maxScriptObjectNameSize = 100;
  constexpr static int k_maxNumberOfDisplayedRows = 6; //240/40
  constexpr static int k_maxScriptNodesCount = 32;
  HighlightCell * leafCellAtIndex(int index) override;
  HighlightCell * nodeCellAtIndex(int index) override { return nullptr; }
  bool selectLeaf(int rowIndex) override;
  void insertTextInCaller(const char * text);
  void addFunctionAtIndex(const char * functionName, int scriptIndex);
  void addVariableAtIndex(const char * variableName, int scriptIndex);
  ScriptNode m_scriptNodes[k_maxScriptNodesCount];
  int m_scriptNodesCount;
  ScriptStore * m_scriptStore;
  ScriptNodeCell m_leafCells[k_maxNumberOfDisplayedRows];
};

}
#endif
