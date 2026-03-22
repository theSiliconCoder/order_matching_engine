# C++ Deterministic Matching Engine & Simulated Trading Clients

## Overview

This project implements a **thread-safe, deterministic order matching engine** in C++ that models the core of a **liquidity-providing market system**.

It includes:

- **Limit order book** with price-time priority
- **Buy and sell order queues** (FIFO processing)
- **Trade execution and partial fills**
- **Threaded client interaction simulation**

While this is a simulation, the architecture mirrors real exchange execution layers and provides a foundation for **market-making and quantitative trading research**.

---

## Key Features

### 1. Deterministic Matching Engine

- Singleton `MatchingEngine` ensures **centralized control** of the order book
- Price-time priority enforced for matching:

  - Highest bid matches lowest ask
  - Partial fills handled correctly

- Thread-safe with **mutexes** and **condition variables**

```cpp
std::mutex orderBookMutex;
std::condition_variable orderQueueCondition;
```

- Pending orders managed via **map + linked list** for O(1) access and efficient queue management

---

### 2. Order Types & Execution

- Supports **basic limit orders** (buy/sell)
- Tracks `pendingAmount` for partial fills
- Orders can be **added, canceled, and monitored**
- Execution triggers **client callbacks** for order placement, cancellation, and trade events

```cpp
Order& bestAsk = sellOrders.begin()->second;
int tradeAmount = std::min(newOrder.pendingAmount, bestAsk.pendingAmount);
newOrder.pendingAmount -= tradeAmount;
bestAsk.pendingAmount -= tradeAmount;
```

- Trades printed as events, showing **order IDs, price, and traded quantity**

---

### 3. Threaded Simulation & Client Interaction

- `TradingClient` instances submit **random or predefined orders**
- Engine continuously processes orders in a **dedicated thread**
- Multiple clients can interact **simultaneously**, modeling a real multi-participant market

```cpp
std::thread matchingProcess(&MatchingEngine::processOrders, &matchingEngine);
```

---

### 4. Order Book Visibility

- Engine maintains **sorted buy/sell books**:

  - Buy orders descending by price
  - Sell orders ascending by price

- Pending orders displayed in **FIFO order**
- Debug-friendly printing for simulation and testing

---

### 5. Extensibility

- Designed for **easy integration** with advanced strategies:

  - Market-making logic
  - Latency-aware execution
  - Risk management and inventory tracking

- Can be extended to **tick-level or exchange-like simulations**

---

## Architectural Highlights

- **Singleton Engine** → ensures only one authoritative order book
- **Locking Strategy** → separates pending orders and order book mutexes to reduce contention
- **Callback Interface** → clients receive real-time order/trade events
- **Flexible Order Representation** → `Order` struct tracks amount, price, pending quantity, and source client

---

## Simulation Example

```cpp
int main() {
    MatchingEngine& engine = MatchingEngine::getInstance();
    TradingClient client;

    // Launch matching engine thread
    std::thread matchingProcess(&MatchingEngine::processOrders, &engine);

    // Multiple clients placing random orders
    std::vector<std::thread> clients;
    for(int i = 0; i < 3; ++i)
        clients.emplace_back(&TradingClient::placeRandomOrder, &client, i+1, 5);

    for(auto& c : clients) c.join();
    matchingProcess.join();
}
```

- Simulates a **multi-client market**
- Processes orders **concurrently and deterministically**
- Produces **trade events and order book snapshots**

---

## Market-Making & Quant Focus

This project demonstrates core **market-making concepts** relevant to Bayse Markets:

1. **Deterministic Price-Time Matching** – ensures fair and predictable execution
2. **Liquidity Provision Simulation** – clients submit both buy/sell orders
3. **Partial Fill Handling** – mirrors real exchange behavior
4. **Thread-Safe Architecture** – models multi-participant markets
5. **Extensible Framework** – can integrate **risk, inventory, and spread management** for full-scale quant strategies

> This positions the system as a foundation for **market-making algorithms, strategy testing, and execution-layer experimentation**.

---

## Compilation & Execution

```bash
make build     # Builds and runs engine
make clean     # Removes binaries
make rebuild   # Cleans and rebuilds
```

Requirements: **C++17**, **pthread support**

---

## Next Steps / Enhancements

- Add **market/limit order types** like IOC, FOK
- Implement **latency and event-driven simulation**
- Track **inventory and PnL for market-making strategies**
- Integrate **realistic price feeds** for backtesting

---

## Conclusion

This engine is production systems worthy:

- It demonstrates **low-latency execution, concurrency control, and order book mechanics**
- Provides a solid foundation for **quantitative research, market-making strategies, and execution system development**
- Shows practical ability to **design systems that model real trading infrastructure**
