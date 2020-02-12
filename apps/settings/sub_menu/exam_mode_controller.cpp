#include "exam_mode_controller.h"
#include "../../global_preferences.h"
#include "../../apps_container.h"
#include "../../exam_mode_configuration.h"
#include <apps/i18n.h>
#include <assert.h>
#include <cmath>
#include <poincare/preferences.h>
#include <apps/i18n.h>

using namespace Poincare;
using namespace Shared;

namespace Settings {

ExamModeController::ExamModeController(Responder * parentResponder) :
  GenericSubController(parentResponder),
  m_contentView(&m_selectableTableView),
  m_cell{}
{
  for (int i = 0; i < k_maxNumberOfCells; i++) {
    m_cell[i].setMessage(ExamModeConfiguration::examModeActivationMessage(i));
    m_cell[i].setMessageFont(KDFont::LargeFont);
  }
}

bool ExamModeController::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK || event == Ion::Events::EXE) {
    AppsContainer::sharedAppsContainer()->displayExamModePopUp(examMode());
    return true;
  }
  return GenericSubController::handleEvent(event);
}

void ExamModeController::didEnterResponderChain(Responder * previousFirstResponder) {
  /* When a pop-up is dismissed, the exam mode status might have changed. We
   * reload the selection as the number of rows might have also changed. We
   * force to reload the entire data because they might have changed. */
  selectCellAtLocation(0, initialSelectedRow());
  m_contentView.reload();
  // We add a message when the mode exam is on
  if (GlobalPreferences::sharedGlobalPreferences()->isInExamMode()) {
    I18n::Message deactivateMessages[] = {I18n::Message::ToDeactivateExamMode1, I18n::Message::ToDeactivateExamMode2, I18n::Message::ToDeactivateExamMode3};
    m_contentView.setMessages(deactivateMessages, k_numberOfDeactivationMessageLines);
  // --------------------- Please don't edit these lines ----------------------
  } else if (numberOfCautionLines() > 0) {
    I18n::Message cautionMessages[] = {I18n::Message::ExamModeWarning1, I18n::Message::ExamModeWarning2, I18n::Message::ExamModeWarning3};
    m_contentView.setMessages(cautionMessages, numberOfCautionLines());
  }
  // --------------------------------------------------------------------------
}

int ExamModeController::numberOfRows() const {
  return ExamModeConfiguration::numberOfAvailableExamMode();
}

HighlightCell * ExamModeController::reusableCell(int index, int type) {
  assert(type == 0);
  assert(index >= 0  && index < 3);
  
  switch(index) {
    case 0:
      return &m_ledCell;
    case 1:
      return &m_modeCell;
    case 2:
      return &m_examModeCell;
  }
}

int ExamModeController::reusableCellCount(int type) {
  assert(type == 0);
  return 3;
}

void ExamModeController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if(index == 0){
    m_examModeCell.setHighlighted(false);
  }
  Preferences * preferences = Preferences::sharedPreferences();
  GenericSubController::willDisplayCellForIndex(cell, index);
  I18n::Message thisLabel = m_messageTreeModel->children(index)->label();

  if (GlobalPreferences::sharedGlobalPreferences()->isInExamMode() && (thisLabel == I18n::Message::ActivateExamMode || thisLabel == I18n::Message::ExamModeActive)) {
    MessageTableCell * myCell = (MessageTableCell *)cell;
    myCell->setMessage(I18n::Message::ExamModeActive);
  }
  if (thisLabel == I18n::Message::LEDColor) {
    MessageTableCellWithChevronAndMessage * myTextCell = (MessageTableCellWithChevronAndMessage *)cell;
    I18n::Message message = (I18n::Message) m_messageTreeModel->children(index)->children((int)preferences->colorOfLED())->label();
    myTextCell->setSubtitle(message);
  }
  if (thisLabel == I18n::Message::ExamModeMode) {
    MessageTableCellWithChevronAndMessage * myTextCell = (MessageTableCellWithChevronAndMessage *)cell;
    I18n::Message message = (I18n::Message) m_messageTreeModel->children(index)->children((uint8_t)GlobalPreferences::sharedGlobalPreferences()->tempExamMode() - 1)->label();
    myTextCell->setSubtitle(message);
  }
}

int ExamModeController::initialSelectedRow() const {
  int row = selectedRow();
  if (row >= numberOfRows()) {
    return numberOfRows()-1;
  } else if (row < 0) {
    return 0;
  }
  return row;
}

GlobalPreferences::ExamMode ExamModeController::examMode() {
  GlobalPreferences::ExamMode mode = ExamModeConfiguration::examModeAtIndex(selectedRow());
  if (GlobalPreferences::sharedGlobalPreferences()->isInExamMode()) {
    // If the exam mode is already on, this re-activate the same exam mode
    mode = GlobalPreferences::sharedGlobalPreferences()->examMode();
  }
  return mode;
}

}
