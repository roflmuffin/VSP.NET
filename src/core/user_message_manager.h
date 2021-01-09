#pragma once

#include <irecipientfilter.h>
#include <utlvector.h>

#include "globals.h"

namespace google {
namespace protobuf {
class Message;
}
}  // namespace google

namespace vspdotnet {

class CustomRecipientFilter : public IRecipientFilter {
 public:
  CustomRecipientFilter();
  ~CustomRecipientFilter();

 public:  // IRecipientFilter
  bool IsReliable() const override;
  bool IsInitMessage() const override;
  int GetRecipientCount() const override;
  int GetRecipientIndex(int slot) const override;
  void MakeInitMessage(void);
  void MakeReliable(void);
  void AddPlayer(int index);
  void RemovePlayer(int index);
  void AddAllPlayers();

 public:
  void Reset();

 private:
  bool m_is_reliable_;
  bool m_is_init_message_;
  CUtlVector<int> m_recipients_;
};


class UserMessageManager : public vspdotnet::GlobalClass {
 public:
  UserMessageManager();
  ~UserMessageManager();
  void OnShutdown() override;
  void OnAllInitialized() override;
  void SendMessageToChat(int client, const char* message);
  void SendHintMessage(int client, const char* message);
  void SendCenterMessage(int client, const char* message);
  void OnSendUserMessage_Pre(IRecipientFilter& filter, int msg_type,
                             const google::protobuf::Message& msg);
  void OnSendUserMessage_Post(IRecipientFilter& filter, int msg_type,
                              const google::protobuf::Message& msg);
  void ShowMenu(int client, int keys, int time, const char* body);

 private:
  int m_text_msg_id_;
  int m_hint_text_msg_id_;
  int m_say_text_msg_id_;
  int m_menu_msg_id_;
};
}