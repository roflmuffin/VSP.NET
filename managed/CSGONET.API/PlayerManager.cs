
//------------------------------------------------------------------------------
// <auto-generated />
//
// This file was automatically generated by SWIG (http://www.swig.org).
// Version 4.0.1
//
// Do not make changes to this file unless you know what you are doing--modify
// the SWIG interface file instead.
//------------------------------------------------------------------------------


using CSGONET.API;
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using CSGONET.API.Modules.Errors;
using CSGONET.API.Modules.Players;

public static class PlayerManager
{
    private static HandleRef handle => new HandleRef(null, NativePINVOKE.g_PlayerManager_get());

    public static int NumPlayers => NativePINVOKE.PlayerManager_NumPlayers(handle);
    public static int MaxClients => NativePINVOKE.PlayerManager_MaxClients(handle);

    public static IEnumerable<Player> GetPlayers()
    {
        List<Player> players = new List<Player>();
        for (int i = 1; i < NumPlayers + 1; i++)
        {
            var player = Player.FromIndex(i);
            if (player != null)
            {
                players.Add(player);
            }
        }

        return players;
    }
}

/*public List<Player> GetPlayers()
{
    List<Player> players = new List<Player>();
    for (int i = 1; i < Native.g_PlayerManager.NumPlayers() + 1; i++)
    {
        var player = Native.g_PlayerManager.GetPlayerByIndex(i);
        if (player != null)
        {
            players.Add(player);
        }
    }

    return players;
}#1#
}
*/
