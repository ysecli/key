#include <iostream>
#include <string>
#include <cstdlib>
using namespace std;
#include "Global.h"
#include "KeyMaterial.h"
#include "CheckDecl.h"


// A correct username + serial prints "Access granted." and nothing else does.

int main()
{
    while (true) {
        system("cls");
        cout << "=============================\n";
        cout << "   THE VAULT :: crackme v1\n";
        cout << "=============================\n";

        if (DebuggerDetected())
        {
            cout << "Not in front of others, I'm shy.\n";
            system("pause");
            continue;
        }

        string user, serial;
        cout << "Username: ";
        getline(cin, user);
        cout << "Serial (X-X-X): ";
        getline(cin, serial);
        
        KeyState state;
        InitKeyState(state, user, serial);

        Gate pipeline[] = {
            Gate::LengthGate,
            Gate::ChecksumGate,
            Gate::XorGate,
            Gate::TransformGate,
            Gate::FinalGate
        };

        bool passed = true;
        for (Gate g : pipeline)
        {
            Result r = RunGate(g, state);
            if (r != Result::Pass)
            {
                cout << "Access denied at " << GateName(g) << ".\n";
                system("pause");
                passed = false;
                break;
            }
        }

        if (passed) {
            cout << "Access granted. Well done.\n";
            system("pause");
        }
    }

    return 0;
}