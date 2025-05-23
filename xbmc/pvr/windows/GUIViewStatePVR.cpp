/*
 *  Copyright (C) 2012-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "GUIViewStatePVR.h"

#include "FileItem.h"
#include "FileItemList.h"
#include "ServiceBroker.h"
#include "pvr/PVRManager.h"
#include "pvr/addons/PVRClients.h"
#include "pvr/epg/EpgSearchPath.h"
#include "pvr/providers/PVRProvidersPath.h"
#include "pvr/recordings/PVRRecordingsPath.h"
#include "pvr/timers/PVRTimersPath.h"
#include "settings/AdvancedSettings.h"
#include "settings/Settings.h"
#include "settings/SettingsComponent.h"
#include "view/ViewStateSettings.h"

using namespace PVR;

CGUIViewStateWindowPVRChannels::CGUIViewStateWindowPVRChannels(const int windowId,
                                                               const CFileItemList& items)
  : CGUIViewStatePVR(windowId, items)
{
  AddSortMethod(SortByChannelNumber, 549, // "Number"
                LABEL_MASKS("%L", "", "%L", "")); // Filename, empty | Foldername, empty
  AddSortMethod(SortByChannel, 551, // "Name"
                LABEL_MASKS("%L", "", "%L", "")); // Filename, empty | Foldername, empty
  AddSortMethod(
      SortByLastPlayed, 568, // "Last played"
      LABEL_MASKS("%L", "%p", "%L", "%p")); // Filename, LastPlayed | Foldername, LastPlayed
  AddSortMethod(SortByDateAdded, 570, // "Date added"
                LABEL_MASKS("%L", "%a", "%L", "%a"), // Filename, DateAdded | Foldername, DateAdded
                SortAttributeNone, SortOrderDescending);
  AddSortMethod(SortByClientChannelOrder, 19315, // "Backend number"
                LABEL_MASKS("%L", "", "%L", "")); // Filename, empty | Foldername, empty
  AddSortMethod(SortByProvider, 19348, // "Provider"
                LABEL_MASKS("%L", "", "%L", "")); // Filename, empty | Foldername, empty

  // Default sorting
  SetSortMethod(SortByChannelNumber);

  LoadViewState("pvr://channels/", GetWindowId());
}

void CGUIViewStateWindowPVRChannels::SaveViewState()
{
  SaveViewToDb("pvr://channels/", GetWindowId(),
               CViewStateSettings::GetInstance().Get("pvrchannels"));
}

CGUIViewStateWindowPVRRecordings::CGUIViewStateWindowPVRRecordings(const int windowId,
                                                                   const CFileItemList& items)
  : CGUIViewStatePVR(windowId, items)
{
  AddSortMethod(SortByLabel, 551, // "Name"
                LABEL_MASKS("%L", "%d", "%L", ""), // Filename, DateTime | Foldername, empty
                CServiceBroker::GetSettingsComponent()->GetSettings()->GetBool(
                    CSettings::SETTING_FILELISTS_IGNORETHEWHENSORTING)
                    ? SortAttributeIgnoreArticle
                    : SortAttributeNone);
  AddSortMethod(SortByDate, 552, // "Date"
                LABEL_MASKS("%L", "%d", "%L", "%d")); // Filename, DateTime | Foldername, DateTime
  AddSortMethod(SortByTime, 180, // "Duration"
                LABEL_MASKS("%L", "%D", "%L", "")); // Filename, Duration | Foldername, empty
  AddSortMethod(SortByFile, 561, // "File"
                LABEL_MASKS("%L", "%d", "%L", "")); // Filename, DateTime | Foldername, empty

  if (CServiceBroker::GetPVRManager().Clients()->AnyClientSupportingRecordingsSize())
  {
    // "Size" : Filename, Size | Foldername, Size
    AddSortMethod(SortBySize, 553, LABEL_MASKS("%L", "%I", "%L", "%I"));
  }

  AddSortMethod(SortByEpisodeNumber, 20359, // "Episode"
                LABEL_MASKS("%L", "%d", "%L", "")); // Filename, DateTime | Foldername, empty
  AddSortMethod(SortByProvider, 19348, // "Provider"
                LABEL_MASKS("%L", "", "%L", "")); // Filename, empty | Foldername, empty

  SetSortMethod(
      CServiceBroker::GetSettingsComponent()->GetAdvancedSettings()->m_PVRDefaultSortOrder);

  LoadViewState(items.GetPath(), GetWindowId());
}

void CGUIViewStateWindowPVRRecordings::SaveViewState()
{
  SaveViewToDb(m_items.GetPath(), GetWindowId(),
               CViewStateSettings::GetInstance().Get("pvrrecordings"));
}

bool CGUIViewStateWindowPVRRecordings::HideParentDirItems()
{
  return (CGUIViewState::HideParentDirItems() ||
          CPVRRecordingsPath(m_items.GetPath()).IsRecordingsRoot());
}

CGUIViewStateWindowPVRGuide::CGUIViewStateWindowPVRGuide(const int windowId,
                                                         const CFileItemList& items)
  : CGUIViewStatePVR(windowId, items)
{
  AddSortMethod(SortByChannelNumber, 549, // "Number"
                LABEL_MASKS("%L", "", "%L", "")); // Filename, empty | Foldername, empty
  AddSortMethod(SortByChannel, 551, // "Name"
                LABEL_MASKS("%L", "", "%L", "")); // Filename, empty | Foldername, empty
  AddSortMethod(
      SortByLastPlayed, SortAttributeIgnoreLabel, 568, // "Last played"
      LABEL_MASKS("%L", "%p", "%L", "%p")); // Filename, LastPlayed | Foldername, LastPlayed
  AddSortMethod(SortByDateAdded, 570, // "Date added"
                LABEL_MASKS("%L", "%a", "%L", "%a"), // Filename, DateAdded | Foldername, DateAdded
                SortAttributeNone, SortOrderDescending);
  AddSortMethod(SortByClientChannelOrder, 19315, // "Backend number"
                LABEL_MASKS("%L", "", "%L", "")); // Filename, empty | Foldername, empty
  AddSortMethod(SortByProvider, 19348, // "Provider"
                LABEL_MASKS("%L", "", "%L", "")); // Filename, empty | Foldername, empty

  // Default sorting
  SetSortMethod(SortByChannelNumber);

  LoadViewState("pvr://guide/", GetWindowId());
}

void CGUIViewStateWindowPVRGuide::SaveViewState()
{
  SaveViewToDb("pvr://guide/", GetWindowId(), CViewStateSettings::GetInstance().Get("pvrguide"));
}

CGUIViewStateWindowPVRTimers::CGUIViewStateWindowPVRTimers(const int windowId,
                                                           const CFileItemList& items)
  : CGUIViewStatePVR(windowId, items)
{
  int sortAttributes(CServiceBroker::GetSettingsComponent()->GetSettings()->GetBool(
                         CSettings::SETTING_FILELISTS_IGNORETHEWHENSORTING)
                         ? SortAttributeIgnoreArticle
                         : SortAttributeNone);
  sortAttributes |= SortAttributeIgnoreFolders;
  AddSortMethod(SortByLabel, static_cast<SortAttribute>(sortAttributes), 551, // "Name"
                LABEL_MASKS("%L", "", "%L", "")); // Filename, empty | Foldername, empty
  AddSortMethod(SortByDate, static_cast<SortAttribute>(sortAttributes), 552, // "Date"
                LABEL_MASKS("%L", "%d", "%L", "%d")); // Filename, DateTime | Foldername, DateTime

  // Default sorting
  SetSortMethod(SortByDate);

  LoadViewState("pvr://timers/", GetWindowId());
}

void CGUIViewStateWindowPVRTimers::SaveViewState()
{
  SaveViewToDb("pvr://timers/", GetWindowId(), CViewStateSettings::GetInstance().Get("pvrtimers"));
}

bool CGUIViewStateWindowPVRTimers::HideParentDirItems()
{
  return (CGUIViewState::HideParentDirItems() || CPVRTimersPath(m_items.GetPath()).IsTimersRoot());
}

CGUIViewStateWindowPVRSearch::CGUIViewStateWindowPVRSearch(const int windowId,
                                                           const CFileItemList& items)
  : CGUIViewStatePVR(windowId, items)
{
  AddSortMethod(SortByLabel, 551, // "Name"
                LABEL_MASKS("%L", "", "%L", "")); // Filename, empty | Foldername, empty
  AddSortMethod(SortByDate, 552, // "Date"
                LABEL_MASKS("%L", "%d", "%L", "%d")); // Filename, DateTime | Foldername, DateTime

  // Default sorting
  if (CPVREpgSearchPath(m_items.GetPath()).IsSavedSearchesRoot())
    SetSortMethod(SortByDate, SortOrderDescending);
  else
    SetSortMethod(SortByDate, SortOrderAscending);

  LoadViewState(m_items.GetPath(), GetWindowId());
}

void CGUIViewStateWindowPVRSearch::SaveViewState()
{
  SaveViewToDb(m_items.GetPath(), GetWindowId(),
               CViewStateSettings::GetInstance().Get("pvrsearch"));
}

bool CGUIViewStateWindowPVRSearch::HideParentDirItems()
{
  return (CGUIViewState::HideParentDirItems() ||
          CPVREpgSearchPath(m_items.GetPath()).IsSearchRoot());
}

CGUIViewStateWindowPVRProviders::CGUIViewStateWindowPVRProviders(const int windowId,
                                                                 const CFileItemList& items)
  : CGUIViewStatePVR(windowId, items)
{
  AddSortMethod(SortByLabel, 551, // "Name"
                LABEL_MASKS("%L", "", "%L", "")); // Filename, empty | Foldername, empty

  if (CPVRProvidersPath(m_items.GetPath()).IsProvidersRoot())
  {
    AddSortMethod(SortByProvider, 19348, // "Provider"
                  LABEL_MASKS("%L", "", "%L", "")); // Filename, empty | Foldername, empty

    SetSortMethod(SortByProvider, SortOrderAscending);
  }
  else
  {
    SetSortMethod(SortByLabel, SortOrderAscending);
  }

  LoadViewState(m_items.GetPath(), GetWindowId());
}

void CGUIViewStateWindowPVRProviders::SaveViewState()
{
  SaveViewToDb(m_items.GetPath(), GetWindowId(),
               CViewStateSettings::GetInstance().Get("pvrproviders"));
}

bool CGUIViewStateWindowPVRProviders::HideParentDirItems()
{
  return (CGUIViewState::HideParentDirItems() ||
          CPVRProvidersPath(m_items.GetPath()).IsProvidersRoot());
}
