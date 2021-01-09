﻿namespace CSGONET.API.Core
{
    /// <summary>
    /// Interface which every CSGONET plugin must implement. Module will be created with parameterless constructor and then Load method will be called.
    /// </summary>
    public interface IPlugin
    {
        /// <summary>
        /// Name of the plugin.
        /// </summary>
        string ModuleName
        {
            get;
        }
        /// <summary>
        /// Module version.
        /// </summary>
        string ModuleVersion
        {
            get;
        }
        /// <summary>
        /// This method is called by CSGONET on plugin load and should be treated as plugin constructor.
        /// </summary>
        void Load(bool hotReload);
        /// <summary>
        /// Will be called by CSGONET on plugin unload. In this method plugin must cleanup and unregister with CSGO native data.
        /// </summary>
        void Unload(bool hotReload);
    }
}