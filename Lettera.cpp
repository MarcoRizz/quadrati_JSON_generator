#include "Lettera.h"

Lettera::Lettera() : fineParola(false) {}

Lettera* Lettera::aggiungiFiglio(char c) {
    if (figli.find(c) == figli.end()) {
        figli[c] = std::make_unique<Lettera>();
    }
    return figli[c].get();
}

Lettera* Lettera::getFiglio(char c) const {
    auto it = figli.find(c);
    if (it != figli.end()) {
        return it->second.get();
    }
    return nullptr;
}

json Lettera::to_json_compatto() const {
    json j;
    j["f"] = fineParola;
    if (!figli.empty()) {
        json figli_json;
        for (const auto& pair : figli) {
            char c = pair.first;
            const std::unique_ptr<Lettera>& figlio = pair.second;
            figli_json[String(1, c)] = figlio->to_json_compatto();
        }
        j["c"] = figli_json;
    }
    return j;
}

std::unique_ptr<Lettera> Lettera::from_json_compatto(const json& j) {
    auto nodo = std::make_unique<Lettera>();
    if (j.contains("f")) {
        nodo->fineParola = j["f"].get<bool>();
    }
    if (j.contains("c")) {
        for (auto it = j["c"].begin(); it != j["c"].end(); ++it) {
            char c = it.key()[0];
            nodo->figli[c] = from_json_compatto(it.value());
        }
    }
    return nodo;
}