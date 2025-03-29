#include "TradingClient.h"
#include <iostream>
#include <random>
#include <thread>

TradingClient::TradingClient() : mEngine(MatchingEngine::getInstance()) {}


void TradingClient::placeRandomOrder(int clientId, int numOrders) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> priceDist(90, 500);
    std::uniform_int_distribution<int> amountDist(10, 50);
    std::uniform_int_distribution<int> buySellDist(0, 1);
    std::uniform_int_distribution<int> timeDist(100, 500);

    for (int i = 0; i < numOrders; ++i) {
        Order newOrder;
        newOrder.id = clientId * 1000 + i;
        newOrder.price = priceDist(gen);
        newOrder.amount = amountDist(gen);
        newOrder.pendingAmount = newOrder.amount;
        newOrder.isBuyOrder = buySellDist(gen);
        newOrder.src = this;

        mEngine.placeOrder(newOrder);

        std::cout << "[Client " << clientId << "] Placed Order: "
                  << (newOrder.isBuyOrder ? "BUY" : "SELL")
                  << " | ID: " << newOrder.id
                  << " | Price: " << newOrder.price
                  << " | Amount: " << newOrder.amount << std::endl;

        // Random delay before next order
        std::this_thread::sleep_for(std::chrono::milliseconds(timeDist(gen)));

        // random order cancellation
        // if (buySellDist(gen) == 1) {
        //     cancelOrder(newOrder.id);
        // }
    }
}

void TradingClient::placeOrder(int id, int price, int amt, bool isBuy) {
    Order newOrder(id, price, amt, isBuy, this);
    mEngine.placeOrder(newOrder);
}

void TradingClient::cancelOrder(int id) {
    mEngine.cancelOrder(id);
}

// CLIENT CALLBACKS
void TradingClient::orderPlaced(Order& order) {
    std::cout << "\nClient Notified: " << order.id << " added to pending Orders\n";
}

void TradingClient::orderCanceled(int orderId) {
    std::cout << "\nClient Notified: " << orderId << " cancel order\n";
}

void TradingClient::orderTraded(Order& order) {
    std::cout << "\nClient Notified: " << order.id << " total traded qty so far: " << order.amount - order.pendingAmount<< "\n";
}
