#include <iostream>
#include <thread>

#include "TradingClient.h"
#include "MatchingEngine.h"



int main (){
    
    MatchingEngine& matchingEngine = MatchingEngine::getInstance();

    TradingClient client;

    // client.placeOrder(3, 15, 90, false);
    // std::this_thread::sleep_for(std::chrono::seconds(2));

    // client.placeOrder(1, 25, 100, true);
    // client.placeOrder(2, 35, 50, true);
    

    // matchingEngine.displayPendingOrders();
    

    std::thread matchingProcess(&MatchingEngine::processOrders, &matchingEngine);




    std::vector<std::thread> clientsList;
    int numClients = 3;
    int ordersPerClient = 5;

    for (int i = 0; i < numClients; ++i) {
        clientsList.emplace_back(&TradingClient::placeRandomOrder, &client, i + 1, ordersPerClient);
    }

    // Join clients
    for (auto& c : clientsList) {
        c.join();
    }

    matchingProcess.join();

    
}