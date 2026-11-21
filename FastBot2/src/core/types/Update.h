#pragma once
#include <Arduino.h>
#include <GSON.h>
#include "MessageRead.h"
#include "QueryRead.h"
#include "../EntryAccess.h"
#include "../api.h"
namespace fb {
class Update : public EntryAccess {
   public:
    enum class Type : size_t {
        Message = tg_apih::message,
        EditedMessage = tg_apih::edited_message,
        ChannelPost = tg_apih::channel_post,
        EditedChannelPost = tg_apih::edited_channel_post,
        BusinessConnection = tg_apih::business_connection,
        BusinessMessage = tg_apih::business_message,
        EditedBusinessMessage = tg_apih::edited_business_message,
        DeletedBusinessMessages = tg_apih::deleted_business_messages,
        MessageReaction = tg_apih::message_reaction,
        MessageReactionCount = tg_apih::message_reaction_count,
        InlineQuery = tg_apih::inline_query,
        ChosenInlineResult = tg_apih::chosen_inline_result,
        CallbackQuery = tg_apih::callback_query,
        ShippingQuery = tg_apih::shipping_query,
        PreCheckoutQuery = tg_apih::pre_checkout_query,
        Poll = tg_apih::poll,
        PollAnswer = tg_apih::poll_answer,
        MyChatMember = tg_apih::my_chat_member,
        ChatMember = tg_apih::chat_member,
        ChatJoinRequest = tg_apih::chat_join_request,
        ChatBoost = tg_apih::chat_boost,
        RemovedChatBoost = tg_apih::removed_chat_boost,
    };
    Update(gson::Entry& entry, size_t type, uint32_t id) : EntryAccess(entry), _type((Type)type), _id(id) {}
    Type type() {
        return _type;
    }
    uint32_t id() {
        return _id;
    }
    bool isQuery() {
        return _type == Type::CallbackQuery;
    }
    QueryRead query() {
        return QueryRead(entry);
    }
    MessageRead message() {
        return MessageRead(entry);
    }
    bool isMessage() {
        return _type == Type::Message || _type == Type::EditedMessage;
    }
    bool isPost() {
        return _type == Type::ChannelPost || _type == Type::EditedChannelPost;
    }
    bool isEdited() {
        return _type == Type::EditedMessage || _type == Type::EditedChannelPost;
    }
   private:
    Type _type;
    uint32_t _id;
};
}