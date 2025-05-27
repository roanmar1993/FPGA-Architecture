#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <deque>
#include <algorithm>
#include <fstream>
#include <sstream>




using namespace std;

// Data structures used to store the network components
vector<int> primaryInputs;
vector<int> primaryOutputs;
map<int, set<int>> gates;

// Function to read the inputs from the file and generate the inputs that will be used later one
void readFromFile(const string& filePath) {
    ifstream file(filePath);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file." << endl;
        return;
    }

    string line;

    // Step 1: Read the first line to read the following inputs (network name, N, I, O)
    getline(file, line);
    istringstream iss(line);
    string networkName;
    int N, I, O;
    iss >> networkName >> N >> I >> O;

    // Step 2: Read primary inputs and save in vector<int> primaryInputs;
    for (int i = 0; i < I; i++) {
        getline(file, line);
        int inputId = stoi(line);
        primaryInputs.push_back(inputId);
    }

    // Step 3: Read primary outputs and save to vector<int> primaryOutputs;
    for (int i = 0; i < O; i++) {
        getline(file, line);
        int outputId = stoi(line);
        primaryOutputs.push_back(outputId);
    }

    // Step 4: Read gate connections and save to map<int, set<int>> gates;
    while (getline(file, line)) {
        istringstream gateIss(line);
        int gateId, input;
        gateIss >> gateId;
        set<int> inputs;

        while (gateIss >> input) {
            inputs.insert(input);
        }

        gates[gateId] = inputs;
    }

    file.close();

}

// Map to store levels of each node, since I start processing the nodes from the deepest level, having a lavel with the right level is crucial
map<int, int> levels;


void assignLevelsOptimized() {
    // Create a reverse adjacency list
    map<int, vector<int>> reverseAdjList;

    // Populate the reverse adjacency list
    for (const auto& [gateId, inputs] : gates) {
        for (int input : inputs) {
            reverseAdjList[input].push_back(gateId);
        }
    }

    deque<int> queue;

    // Initialize levels for primary inputs
    for (int input : primaryInputs) {
        levels[input] = 1;
        queue.push_back(input);
    }

    // Perform BFS using the reverse adjacency list to assign levels 
    while (!queue.empty()) {
        int current = queue.front();
        queue.pop_front();

        // Directly access dependent gates using the reverse adjacency list
        if (reverseAdjList.find(current) != reverseAdjList.end()) {
            for (int gateId : reverseAdjList[current]) {
                levels[gateId] = max(levels[gateId], levels[current] + 1);
                queue.push_back(gateId);
            }
        }
    }

    // Print the levels for debugging
    // cout << "\nAssigned Levels:" << endl;
    // for (const auto& [node, level] : levels) {
    //     cout << "Node " << node << " -> Level " << level << endl;
    // }
    // cout << "Finished labeling" << endl;
}


vector<pair<int, set<int>>> generateLUTs(
    const vector<int>& primaryOutputs,
    const map<int, set<int>>& gates,
    const map<int, int>& levels,
    const vector<int>& primaryInputs,
    int K
) {
    vector<pair<int, set<int>>> luts;
    set<int> visited;
    set<int> mergedNodes;
    set<int> inputTracker;

    auto expandLUT = [&](set<int>& lutSet, int node, set<int>& expandedSet) {
        if (find(primaryInputs.begin(), primaryInputs.end(), node) != primaryInputs.end()) {
            return lutSet;
        }
        if (gates.find(node) != gates.end() && expandedSet.find(node) == expandedSet.end()) {
            set<int> newInputs = gates.at(node);
            expandedSet.insert(node);
            lutSet.erase(node);
            lutSet.insert(newInputs.begin(), newInputs.end());

            // Debugging: Track LUT expansion
            // cout << "Processing input " << node << " for LUT expansion. Updated LUT: { ";
            // for (int inp : lutSet) cout << inp << " ";
            // cout << "}" << endl;
        }
        return lutSet;
    };

auto createLUT = [&](int node) -> vector<int> {
    if (visited.find(node) != visited.end() || find(primaryInputs.begin(), primaryInputs.end(), node) != primaryInputs.end()) {
        return {};
    }

    // cout << "\nStarting LUT creation for Node " << node << endl;

    // Initialize LUT set with the direct inputs of the node
    set<int> lutSet(gates.at(node).begin(), gates.at(node).end());
    set<int> expandedInputs;
    set<int> tempMergedNodes;
    deque<int> queue(gates.at(node).begin(), gates.at(node).end());

    set<int> previousLutSet = lutSet; // Store previous state for backtracking, in case we need to revert if number of inputs exceed K

    while (!queue.empty()) {
        // Sort queue based on levels (highest level first)
        sort(queue.begin(), queue.end(), [&](int a, int b) {
            return levels.at(a) > levels.at(b);
        });

        int currentNode = queue.front();
        queue.pop_front();

        // cout << "Processing input " << currentNode << " for Node " << node << endl;

        // Attempt to expand the LUT with the current node
        set<int> expandedLut = expandLUT(lutSet, currentNode, expandedInputs);

        // Check if the LUT exceeds the size limit K
        if (expandedLut.size() > K) {
            // cout << "LUT for Node " << node << " reached maximum size " << K << ". Reverting to previous state.\n";
            lutSet = previousLutSet; // Revert to previous state
            break;
        }

        // Update the LUT and store the current state for potential backtracking
        previousLutSet = lutSet;
        lutSet = expandedLut;

        if (gates.find(currentNode) != gates.end()) {
            for (int inp : gates.at(currentNode)) {
                queue.push_back(inp);
            }
        }
        tempMergedNodes.insert(currentNode);

        // Tracks the LUT state
        // cout << "Updated LUT for Node " << node << ": { ";
        // for (int inp : lutSet) cout << inp << " ";
        // cout << "}" << endl;
    }

    // If the final LUT size is within the limit, finalize the LUT
    if (lutSet.size() <= K) {
        mergedNodes.insert(tempMergedNodes.begin(), tempMergedNodes.end());
        visited.insert(node);

        for (int inputNode : lutSet) {
            inputTracker.insert(inputNode);
        }

        // cout << "Finalized LUT for Node " << node << ": { ";
        // for (int inp : lutSet) cout << inp << " ";
        // cout << "}" << endl;

        return vector<int>(lutSet.begin(), lutSet.end());
    }

    // If the LUT exceeds K, discard it
    // cout << "Discarding LUT for Node " << node << " as it exceeds the maximum size." << endl;
    return {};
};


    map<int, vector<int>> nodesByLevel;
    for (const auto& [node, level] : levels) {
        nodesByLevel[level].push_back(node);
    }

    for (auto it = nodesByLevel.rbegin(); it != nodesByLevel.rend(); ++it) {
        // cout << "\nProcessing Level " << level << endl;
        for (int node : it->second) {
            if (visited.find(node) != visited.end() || 
                (mergedNodes.find(node) != mergedNodes.end() && inputTracker.find(node) == inputTracker.end())) {
                // cout << "Skipping Node " << node << " as it's already visited or merged" << endl;
                continue;
            }

            vector<int> lut = createLUT(node);
            if (!lut.empty()) {
                luts.push_back({node, set<int>(lut.begin(), lut.end())});
                for (int inp : lut) inputTracker.insert(inp);
            }
        }
    }
    // sometimes some nodes can be prematurely merged and if these become inputs of other ndoes, that means a LUT must be create for them as well
    for (int node : inputTracker) {
        if (visited.find(node) == visited.end() && 
            find(primaryInputs.begin(), primaryInputs.end(), node) == primaryInputs.end() &&
            mergedNodes.find(node) != mergedNodes.end()) {

            // cout << "Creating additional LUT for input node " << node << endl;
            vector<int> lut = createLUT(node);
            if (!lut.empty()) {
                luts.push_back({node, set<int>(lut.begin(), lut.end())});
            }
        }
    }

    return luts;
}




void saveLUTsToFile(const vector<pair<int, set<int>>>& luts, const string& filename) {
    // Open the output file in write mode
    ofstream outFile(filename);

    // Check if the file is opened successfully
    if (!outFile.is_open()) {
        cerr << "Error: Could not open the file " << filename << endl;
        return;
    }

    // Write each LUT to the file in the specified format
    for (const auto& lut : luts) {
        outFile << lut.first; // Write the output ID
        for (const int input : lut.second) {
            outFile << " " << input; // Write each input
        }
        outFile << endl; // Move to the next line
    }

    // Close the file
    outFile.close();
    cout << "LUTs have been saved to " << filename << endl;
}

int main(int argc, char* argv[]) {


    //error handling part
    if (argc != 4) {
        cerr << "Usage: ./mapper <input file path> <output file path> <K>" << endl;
        return 1;
    }

    string inputFilePath = argv[1];
    string outputFilePath = argv[2];
    int K = stoi(argv[3]);
    //reading input file
    readFromFile(inputFilePath);
    //assign levels to nodes
    assignLevelsOptimized();
    vector<pair<int, set<int>>> luts = generateLUTs(primaryOutputs, gates, levels, primaryInputs, K);
 

    // Save the generated LUTs to the file
    saveLUTsToFile(luts, outputFilePath);


    return 0;
}
