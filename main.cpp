#include "token.h"
#include "other-shit.h"
#include <dpp/dispatcher.h>
#include <dpp/dpp.h>
#include <vector>
#include <ctime>
#include <string>
#include <chrono>
#include <cstdlib>
#include <iostream>
    
typedef unsigned int uint;

std::vector<dpp::user> users_to_undistract;

namespace undistract
{
    dpp::user last_undistract_request_user;
    void addingToUndistractorListNotice(const dpp::user& user, const dpp::slashcommand_t& event)
    {
        bool added = 0;
        std::string final_message;
        for (int i = 0; i < users_to_undistract.size(); i++)
        {
            if (users_to_undistract[i] == user) {added = 1; break;}
        }

        if (added) {final_message = "Added " + user.username + " to the list.";}

        else {final_message = "Removed " + user.username + " from the list.";}
        event.from()->creator->message_create(dpp::message(event.command.channel_id, final_message));
    }
    void addToUndistractList(const dpp::user& user)
    {
        if (user.id == BOT_ID) return;

        bool add_or_remove = 0; // 0 is add
        if (users_to_undistract.size() != 0)
        {
            for (int i = 0; i < users_to_undistract.size(); i++)
            {
                if (users_to_undistract[i] == user) {add_or_remove = 1; break;}
            }
        }
        if (!add_or_remove) 
        {            
            users_to_undistract.push_back(user); 
            return;
        }
        
        for (int i = 0; i < users_to_undistract. size(); i++)
        {
            if (users_to_undistract[i] == user) {remove(users_to_undistract.begin(), users_to_undistract.end(), users_to_undistract[i]); users_to_undistract.pop_back();}
        }
    }
    void undistract(const dpp::message& msg, const dpp::message_create_t& event, dpp::cluster& bot)
    {
        for (int i = 0; i < users_to_undistract.size(); i++)
        {
            if (msg.author != users_to_undistract[i]) continue;

            std::string reply_message = "Turn the app off " + msg.author.username;
            event.from()->creator->message_create(dpp::message(event.msg.channel_id, reply_message));
            bot.message_delete(msg.id, msg.channel_id);
        }

    }
};

void fuckGlados(const dpp::message& msg, const dpp::message_create_t& event)
{
    uint flip_off_glados = rand() % 5;
    srand(time(0));

    if (msg.author.id == GLADOS_ID) std::cout << "\nStupid clanker replied to someone\n";
    else return;
    
    if (flip_off_glados == 0)
    {
        srand(time(0));
        size_t pool_size = sizeof(clanker_insult_pool)/sizeof(clanker_insult_pool[0]);
        const std::string& chosen_insult = clanker_insult_pool[rand() % pool_size];
        event.reply(chosen_insult);
        std::cout << "\nInsulted the stupid clanker lmao" << std::endl;
        return;
    }
    std::cout << "\nClanker got away this time" << std::endl;
}

void ragebait(const dpp::message_create_t& event)
{
    srand(time(0));
    uint random = rand() % 50;

    if (random) {std::cout << "message not getting ragebaited" << std::endl; return;}

    size_t pool_size = sizeof(ragebait_pool)/sizeof(ragebait_pool[0]);

    srand(time(0));
    size_t picked_ragebait = rand() % pool_size;
    event.reply(ragebait_pool[picked_ragebait]);
    std::cout << "ragebatinig this guy lmao" << std::endl;
}

void logMessage(const dpp::message& msg)
{
    time_t now = time(0);
    char* date_time = ctime(&now);
    std::cout << "\nMessage sent by: " << msg.author.username << "\n";
    std::cout << "Sent at: " << date_time;
    std::cout << "Message content: " << msg.content << std::endl;  
}


int main() 
{    
    users_to_undistract.resize(0);

    uint64_t intents = dpp::i_default_intents | dpp::i_message_content;

    dpp::cluster bot(BOT_TOKEN, intents);

    bot.on_log(dpp::utility::cout_logger());
    
    bot.on_slashcommand([](const dpp::slashcommand_t& event) 
    {
        const dpp::user author = event.command.get_issuing_user();

        std::string command_name = event.command.get_command_name();
        if (command_name == "undistractor") 
        {

            
            event.reply("Adding you to the undistractor list...");
            undistract::addToUndistractList(author);
            undistract::addingToUndistractorListNotice(author, event);

            //Notes:
            //Event.command.message is the bot's reply message
            //Get_issuing_user gets the user 
        }
        else if (command_name == "saytime")
        {
            auto now = std::chrono::system_clock::now();
            std::time_t t = std::chrono::system_clock::to_time_t(now);
            std::tm tm;
            localtime_r(&t, &tm);
            int hour = tm.tm_hour;
            event.reply(std::to_string(hour));
        }
        else if (command_name == "laughatthisguy")
        {
            std::string final_message;
            if (author.id != POLAR_ID) 
            {
                event.reply("I only obey my creator.");
                return;
            }
            event.reply("LMAOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO");
        }
    });
    
    bot.on_ready([&bot](const dpp::ready_t& event) {
        if (dpp::run_once<struct register_bot_commands>()) {
            bot.global_command_create(dpp::slashcommand("undistractor", "For idiot to focus", bot.me.id));
            bot.global_command_create(dpp::slashcommand("saytime", "Gives the current time", bot.me.id));
            bot.global_command_create(dpp::slashcommand("laughatthisguy", "for polar to laugh at people", bot.me.id));
        }
    });
    
    bot.on_message_create([&bot](const dpp::message_create_t& event)
    {
        srand(time(0));
        const dpp::message& msg = event.msg;

        if (msg.author.id != BOT_ID) undistract::undistract(msg, event, bot);
        logMessage(msg);
        fuckGlados(msg, event);
        if (msg.channel_id == GENERAL_CHANNEL_ID && msg.author.id != BOT_ID) ragebait(event);
    });

    bot.start(dpp::st_wait);
}
