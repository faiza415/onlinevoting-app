#include <wx/wx.h>
#include <wx/choice.h>
#include <vector>
#include <string>
#include <regex>
#include <unordered_map>
#include <algorithm> // For max_element

// Linked List for storing voter information
struct Voter {
    std::string name;
    std::string cnic;
    int age;
    std::string province;
    Voter* next;

    Voter(const std::string& n, const std::string& c, int a, const std::string& p) :
        name(n), cnic(c), age(a), province(p), next(nullptr) {
    }
};

// Binary Search Tree for storing voting options and votes
struct TreeNode {
    std::string option;
    int votes;
    TreeNode* left;
    TreeNode* right;

    TreeNode(const std::string& opt) : option(opt), votes(0), left(nullptr), right(nullptr) {}
};

class VotingSystem : public wxFrame {
public:
    VotingSystem() : wxFrame(NULL, wxID_ANY, "Voting System", wxDefaultPosition, wxSize(600, 400)) {
        // Initialize the BST with voting options
        for (const std::string& option : { "Football", "Bat", "Ball", "Ludo" }) {
            root = InsertOption(root, option);
        }

        // Create a panel and sizers for the layout
        wxPanel* panel = new wxPanel(this);

        // Set the background color of the panel to dark green
        panel->SetBackgroundColour(wxColour(0, 100, 0)); // RGB values for dark green

        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

        // Add a title
        wxStaticText* title = new wxStaticText(panel, wxID_ANY, "Online Voting System", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
        wxFont titleFont(18, wxFONTFAMILY_ROMAN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, "Times New Roman");
        title->SetFont(titleFont);
        title->SetForegroundColour(wxColour(255, 255, 255)); // Set text color to white for better contrast
        mainSizer->Add(title, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 20);

        // Create a font for the buttons and dialogs
        wxFont buttonFont(12, wxFONTFAMILY_ROMAN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Times New Roman");

        // Create buttons for actions
        wxButton* registerButton = new wxButton(panel, wxID_ANY, "Register & Vote");
        registerButton->SetFont(buttonFont);

        wxButton* adminButton = new wxButton(panel, wxID_ANY, "Admin Functions");
        adminButton->SetFont(buttonFont);

        wxButton* exitButton = new wxButton(panel, wxID_ANY, "Exit");
        exitButton->SetFont(buttonFont);

        // Create a sizer for the buttons
        wxBoxSizer* buttonSizer = new wxBoxSizer(wxVERTICAL);
        buttonSizer->Add(registerButton, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 10);
        buttonSizer->Add(adminButton, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 10);
        buttonSizer->Add(exitButton, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 10);

        // Add buttonSizer to the mainSizer
        mainSizer->Add(buttonSizer, 1, wxALIGN_CENTER);

        // Set sizer for the panel
        panel->SetSizerAndFit(mainSizer);

        // Center the frame on the screen
        Centre(); // This will center the window

        // Event bindings
        registerButton->Bind(wxEVT_BUTTON, &VotingSystem::OnRegisterVote, this);
        adminButton->Bind(wxEVT_BUTTON, &VotingSystem::OnAdminFunctions, this);
        exitButton->Bind(wxEVT_BUTTON, &VotingSystem::OnExit, this);
    }

private:
    Voter* voterList = nullptr; // Linked list of voters
    TreeNode* root = nullptr;   // Root of the BST
    std::unordered_map<std::string, Voter*> voterMap; // Hash map for quick CNIC lookup

    // Insert a voting option into the BST
    TreeNode* InsertOption(TreeNode* node, const std::string& option) {
        if (!node) return new TreeNode(option);
        if (option < node->option) node->left = InsertOption(node->left, option);
        else if (option > node->option) node->right = InsertOption(node->right, option);
        return node;
    }

    // Find a node in the BST
    TreeNode* FindOption(TreeNode* node, const std::string& option) {
        if (!node || node->option == option) return node;
        if (option < node->option) return FindOption(node->left, option);
        return FindOption(node->right, option);
    }

    // Register and vote
    void OnRegisterVote(wxCommandEvent& event) {
        wxFont dialogFont(12, wxFONTFAMILY_ROMAN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Times New Roman");

        wxTextEntryDialog nameDialog(this, "Enter your name", "Voter Registration", "", wxOK | wxCANCEL);
        nameDialog.SetFont(dialogFont);
        if (nameDialog.ShowModal() == wxID_OK) {
            std::string name = nameDialog.GetValue().ToStdString();
            wxTextEntryDialog cnicDialog(this, "Enter your CNIC", "Voter Registration", "", wxOK | wxCANCEL);
            cnicDialog.SetFont(dialogFont);
            if (cnicDialog.ShowModal() == wxID_OK) {
                std::string cnic = cnicDialog.GetValue().ToStdString();
                if (!IsValidCNIC(cnic)) {
                    wxMessageDialog(this, "Invalid CNIC. Please enter a valid 13-digit CNIC.", "Error", wxOK).ShowModal();
                    return;
                }
                if (voterMap.find(cnic) != voterMap.end()) {
                    wxMessageDialog(this, "This CNIC is already registered.", "Error", wxOK).ShowModal();
                    return;
                }
                wxTextEntryDialog ageDialog(this, "Enter your age", "Voter Registration", "", wxOK | wxCANCEL);
                ageDialog.SetFont(dialogFont);
                if (ageDialog.ShowModal() == wxID_OK) {
                    int age = wxAtoi(ageDialog.GetValue());
                    if (age >= 18) {
                        wxArrayString provinces;
                        provinces.Add("Khyber Pakhtunkhwa");
                        provinces.Add("FATA");
                        provinces.Add("Punjab");
                        provinces.Add("Sindh");
                        provinces.Add("Balochistan");
                        provinces.Add("Islamabad");
                        provinces.Add("Gilgit-Baltistan");

                        wxSingleChoiceDialog provinceDialog(this, "Select your province", "Province", provinces);
                        provinceDialog.SetFont(dialogFont);
                        if (provinceDialog.ShowModal() == wxID_OK) {
                            std::string province = provinceDialog.GetStringSelection().ToStdString();

                            // Add voter to the linked list and hash map
                            Voter* newVoter = new Voter(name, cnic, age, province);
                            newVoter->next = voterList;
                            voterList = newVoter;
                            voterMap[cnic] = newVoter;

                            wxArrayString choices;
                            std::vector<std::string> options = { "Football", "Bat", "Ball", "Ludo" };
                            for (const auto& option : options) {
                                choices.Add(option);
                            }

                            wxSingleChoiceDialog voteDialog(this, "Select your vote", "Voting", choices);
                            voteDialog.SetFont(dialogFont);
                            if (voteDialog.ShowModal() == wxID_OK) {
                                std::string selectedVote = voteDialog.GetStringSelection().ToStdString();
                                TreeNode* voteNode = FindOption(root, selectedVote);
                                if (voteNode) voteNode->votes++;
                                wxMessageDialog(this, "Vote registered successfully!", "Vote", wxOK).ShowModal();
                            }
                        }
                    }
                    else {
                        wxMessageDialog(this, "You must be 18 or older to vote.", "Voter Registration", wxOK).ShowModal();
                    }
                }
            }
        }
    }

    // CNIC validation function
    bool IsValidCNIC(const std::string& cnic) {
        return cnic.length() == 13 && std::regex_match(cnic, std::regex("^[0-9]{13}$"));
    }

    // Admin functions
    void OnAdminFunctions(wxCommandEvent& event) {
        wxFont dialogFont(12, wxFONTFAMILY_ROMAN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Times New Roman");

        wxArrayString choices;
        choices.Add("Authenticate Voters");
        choices.Add("Show Voters Quantity");
        choices.Add("Show Combined Graph");
        choices.Add("Show Winning Option");

        wxSingleChoiceDialog adminDialog(this, "Select admin function", "Admin Functions", choices);
        adminDialog.SetFont(dialogFont);
        if (adminDialog.ShowModal() == wxID_OK) {
            int choice = adminDialog.GetSelection();
            switch (choice) {
            case 0:
                wxMessageDialog(this, "Voters who are 18 or older are eligible to vote.", "Authentication", wxOK).ShowModal();
                break;
            case 1:
                ShowVotersQuantity();
                break;
            case 2:
                ShowCombinedGraph();
                break;
            case 3:
                ShowWinningOption();
                break;
            }
        }
    }

    // Show voter statistics
    void ShowVotersQuantity() {
        std::string result;
        ShowVotes(root, result);
        wxMessageDialog(this, result, "Voter Statistics", wxOK).ShowModal();
    }

    void ShowVotes(TreeNode* node, std::string& result) {
        if (!node) return;
        ShowVotes(node->left, result);
        result += node->option + ": " + std::to_string(node->votes) + " votes\n";
        ShowVotes(node->right, result);
    }

    // Show the combined voting graph
    void ShowCombinedGraph() {
        try {
            // Define options and their votes
            std::vector<std::string> options = { "Football", "Bat", "Ball", "Ludo" };
            std::vector<int> votes = { 0, 0, 0, 0 };

            // Collect votes from the BST
            CollectVotes(root, options, votes);

            // Create a dialog to display the graph
            wxDialog graphDialog(this, wxID_ANY, "Voting Graph", wxDefaultPosition, wxSize(600, 400));

            // Add a panel to the dialog for drawing the graph
            wxPanel* graphPanel = new wxPanel(&graphDialog, wxID_ANY, wxDefaultPosition, wxSize(600, 400));
            graphPanel->SetBackgroundColour(*wxWHITE);

            // Bind the paint event to the panel
            graphPanel->Bind(wxEVT_PAINT, [=](wxPaintEvent&) {
                wxPaintDC dc(graphPanel);

                // Get the panel dimensions
                int panelWidth, panelHeight;
                graphPanel->GetClientSize(&panelWidth, &panelHeight);

                // Define margins and graph dimensions
                int margin = 50;
                int graphWidth = panelWidth - 2 * margin;
                int graphHeight = panelHeight - 2 * margin;

                // Find the maximum number of votes for scaling
                int maxVotes = *std::max_element(votes.begin(), votes.end());
                if (maxVotes == 0) maxVotes = 1; // Avoid division by zero

                // Draw axes
                dc.SetPen(*wxBLACK_PEN);
                dc.DrawLine(margin, margin, margin, panelHeight - margin);            // Y-axis
                dc.DrawLine(margin, panelHeight - margin, panelWidth - margin, panelHeight - margin); // X-axis

                // Define the colors for the bars
                wxColour barColors[] = { wxColour(255, 0, 0), wxColour(0, 255, 0), wxColour(255, 255, 0), wxColour(0, 0, 255) };

                // Draw bars for each voting option
                int barWidth = graphWidth / (votes.size() * 2);
                for (size_t i = 0; i < votes.size(); ++i) {
                    int barHeight = (votes[i] * graphHeight) / maxVotes;
                    int xPos = margin + i * (2 * barWidth) + barWidth / 2;
                    int yPos = panelHeight - margin - barHeight;

                    // Set a unique color for each bar
                    dc.SetBrush(barColors[i]);
                    dc.DrawRectangle(xPos, yPos, barWidth, barHeight);

                    // Add the option name below the bar
                    dc.DrawText(wxString::Format("%s", options[i]), xPos, panelHeight - margin + 5);
                }
                });

            // Show the dialog
            graphDialog.ShowModal();
        }
        catch (const std::exception& e) {
            wxMessageBox(wxString::Format("Error: %s", e.what()), "Exception Caught", wxICON_ERROR);
        }
        catch (...) {
            wxMessageBox("An unknown error occurred.", "Exception Caught", wxICON_ERROR);
        }
    }

    // Helper function to collect votes from the BST
    void CollectVotes(TreeNode* node, std::vector<std::string>& options, std::vector<int>& votes) {
        if (node) {
            auto it = std::find(options.begin(), options.end(), node->option);
            if (it != options.end()) {
                votes[it - options.begin()] = node->votes;
            }
            CollectVotes(node->left, options, votes);
            CollectVotes(node->right, options, votes);
        }
    }

    // Show the winning option
    void ShowWinningOption() {
        TreeNode* winner = FindWinner(root);
        if (winner) {
            wxMessageDialog(this, "Winning Option: " + winner->option + " with " + std::to_string(winner->votes) + " votes.", "Winning Option", wxOK).ShowModal();
        }
        else {
            wxMessageDialog(this, "No votes have been cast yet.", "Winner", wxOK).ShowModal();
        }
    }

    // Helper function to find the winning option
    TreeNode* FindWinner(TreeNode* node) {
        if (!node) return nullptr;
        TreeNode* leftWinner = FindWinner(node->left);
        TreeNode* rightWinner = FindWinner(node->right);
        TreeNode* currentWinner = node;

        if (leftWinner && leftWinner->votes > currentWinner->votes) {
            currentWinner = leftWinner;
        }
        if (rightWinner && rightWinner->votes > currentWinner->votes) {
            currentWinner = rightWinner;
        }
        return currentWinner;
    }

    // Exit the application
    void OnExit(wxCommandEvent& event) {
        Close(true);
    }
};

// Main application class
class VotingApp : public wxApp {
public:
    virtual bool OnInit() {
        VotingSystem* votingSystem = new VotingSystem();
        votingSystem->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(VotingApp);
