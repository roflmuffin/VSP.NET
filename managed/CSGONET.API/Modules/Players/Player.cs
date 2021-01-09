﻿using System;
using System.Collections.Generic;
using System.Security.Cryptography;
using CSGONET.API.Core;
using CSGONET.API.Modules.Entities;
using CSGONET.API.Modules.Entities.Constants;
using CSGONET.API.Modules.Utils;

namespace CSGONET.API.Modules.Players
{
    public class Player : BaseEntity
    {
        internal Player(IntPtr pointer) : base(pointer)
        {
        }

        public new static Player FromIndex(int index)
        {
            var playerBaseEntity = BaseEntity.FromIndex(index);
            if (playerBaseEntity != null && !playerBaseEntity.IsPlayer) return null;
            if (playerBaseEntity == null) return null;
            return new Player(playerBaseEntity.Handle);
        }

        private PlayerInfo _playerInfo;

        public PlayerInfo PlayerInfo => _playerInfo ??
                                        new PlayerInfo(NativeAPI.PlayerinfoFromIndex(Index));
        public bool IsAlive => LifeState == (int)Entities.Constants.LifeState.LIFE_ALIVE;

        public ObserverMode ObserverMode
        {
            get => (ObserverMode)GetProp(PropType.Send, "m_iObserverMode");
            set => SetProp(PropType.Send, "m_iObserverMode", (int)value);
        }

        public PlayerButtons Buttons => (PlayerButtons)GetProp(PropType.Data, "m_nButtons");
        public IEnumerable<PlayerButtons> SelectedButtons => Buttons.FlagsToList();

        public string Name
        {
            get => PlayerInfo.Name;
        }

        public Player ObserverTarget
        {
            get
            {
                var targetIndex= GetPropEnt(PropType.Send, "m_hObserverTarget")?.Index;
                return targetIndex != null ? Player.FromIndex(targetIndex.Value) : null;
            }
        }


        public bool IsScoped
        {
            get => GetPropBool(PropType.Send, "m_bIsScoped");
            set => SetPropBool(PropType.Send, "m_bIsScoped", value);
        }

        public EntityFlags Flags
        {
            get => (EntityFlags)GetProp(PropType.Data, "m_fFlags");
            set => SetProp(PropType.Data, "m_fFlags", (int)value);
        }

        public int LifeState
        {
            get => GetProp(PropType.Send, "m_lifeState");
            set => SetProp(PropType.Send, "m_lifeState", value);
        }

        public Vector ViewOffset
        {
            get => GetPropVector(PropType.Data, "m_vecViewOffset");
            set => SetPropVector(PropType.Data, "m_vecViewOffset", value);
        }

        public Vector EyeLocation => Origin + ViewOffset;

        public Angle EyeAngle
        {
            get
            {
                try
                {
                    var x = GetPropFloat(PropType.Send, "m_angEyeAngles[0]");
                    var y = GetPropFloat(PropType.Send, "m_angEyeAngles[1]");
                    return new Angle(x, y, 0);
                }
                catch (Exception e)
                {
                    Server.PrintToConsole($"Error when retrieving eye angles: {e.Message}, {e.StackTrace}");
                    return new Angle();
                }
            }
        }

        public Vector ViewVector
        {
            get
            {
                double yaw = (Math.PI / 180) * EyeAngle.Y;
                double pitch = (Math.PI / 180) * EyeAngle.X;

                var sy = Math.Sin(yaw);
                var cy = Math.Cos(yaw);

                var sp = Math.Sin(pitch);
                var cp = Math.Cos(pitch);

                return new Vector((float)(cp * cy), (float)(cp * sy), (float)-sp);
            }
        }

        public NetInfo NetInfo => NativePINVOKE.GetPlayerNetInfo(Index).ToObject<NetInfo>();

        public BaseEntity ActiveWeapon
        {
            get => GetPropEnt(PropType.Data, "m_hActiveWeapon");
            set => SetPropEnt(PropType.Data, "m_hActiveWeapon", value.Index);
        }

        public static Player FromUserId(int userid)
        {
            var index = NativeAPI.IndexFromUserid(userid);
            if (index <= 0) return null;
            return FromIndex(Convert.ToInt32(index));
        }

        public void PrintToChat(string message) => NativeAPI.PrintToChat(Index, message);
        public void PrintToHint(string message) => NativeAPI.PrintToHint(Index, message);
    }
}