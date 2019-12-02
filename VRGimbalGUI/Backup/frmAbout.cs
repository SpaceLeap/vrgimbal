using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;
using System.Reflection;

namespace VRGimbalGUI
{
    partial class frmAbout : Form
    {
        public frmAbout()
        {
            InitializeComponent();

            //  Inizializzare AboutBox per visualizzare le informazioni sul prodotto dalle informazioni dell'assembly.
            //  Modificare le impostazioni delle informazioni dell'assembly per l'applicazione utilizzando:
            //  - Progetto->Proprietà->Applicazione->Informazioni assembly
            //  - AssemblyInfo.cs
            this.Text = String.Format("Informazioni su {0}", AssemblyTitle);
            this.labelProductName.Text = AssemblyProduct;
            this.labelVersion.Text = String.Format("Versione {0}", AssemblyVersion);
            this.labelCopyright.Text = AssemblyCopyright;
            this.labelCompanyName.Text = AssemblyCompany;
            this.textBoxDescription.Text = AssemblyDescription;
        }

        #region Funzioni di accesso attributo assembly

        public string AssemblyTitle
        {
            get
            {
                // Ottiene tutti gli attributi Title di questo assembly
                object[] attributes = Assembly.GetExecutingAssembly().GetCustomAttributes(typeof(AssemblyTitleAttribute), false);
                // Se è presente almeno un attributo Title
                if (attributes.Length > 0)
                {
                    // Seleziona il primo
                    AssemblyTitleAttribute titleAttribute = (AssemblyTitleAttribute)attributes[0];
                    // Se non è una stringa vuota, lo restituisce
                    if (titleAttribute.Title != "")
                        return titleAttribute.Title;
                }
                // Se l'attributo Title non è presente o è costituito da una stringa vuota, restituisce il nome dell'eseguibile
                return System.IO.Path.GetFileNameWithoutExtension(Assembly.GetExecutingAssembly().CodeBase);
            }
        }

        public string AssemblyVersion
        {
            get
            {
                return Assembly.GetExecutingAssembly().GetName().Version.ToString();
            }
        }

        public string AssemblyDescription
        {
            get
            {
                // Ottiene tutti gli attributi Description di questo assembly
                object[] attributes = Assembly.GetExecutingAssembly().GetCustomAttributes(typeof(AssemblyDescriptionAttribute), false);
                // Se l'attributo Description non è presente, restituisce una stringa vuota
                if (attributes.Length == 0)
                    return "";
                // Se l'attributo Description è presente, ne restituisce il valore
                return ((AssemblyDescriptionAttribute)attributes[0]).Description;
            }
        }

        public string AssemblyProduct
        {
            get
            {
                // Ottiene tutti gli attributi Product di questo assembly
                object[] attributes = Assembly.GetExecutingAssembly().GetCustomAttributes(typeof(AssemblyProductAttribute), false);
                // Se l'attributo Product non è presente, restituisce una stringa vuota
                if (attributes.Length == 0)
                    return "";
                // Se l'attributo Product è presente, ne restituisce il valore
                return ((AssemblyProductAttribute)attributes[0]).Product;
            }
        }

        public string AssemblyCopyright
        {
            get
            {
                // Ottiene tutti gli attributi Copyright di questo assembly
                object[] attributes = Assembly.GetExecutingAssembly().GetCustomAttributes(typeof(AssemblyCopyrightAttribute), false);
                // Se l'attributo Copyright non è presente, restituisce una stringa vuota
                if (attributes.Length == 0)
                    return "";
                // Se l'attributo Copyright è presente, ne restituisce il valore
                return ((AssemblyCopyrightAttribute)attributes[0]).Copyright;
            }
        }

        public string AssemblyCompany
        {
            get
            {
                // Ottiene tutti gli attributi Company di questo assembly
                object[] attributes = Assembly.GetExecutingAssembly().GetCustomAttributes(typeof(AssemblyCompanyAttribute), false);
                // Se l'attributo Company non è presente, restituisce una stringa vuota
                if (attributes.Length == 0)
                    return "";
                // Se l'attributo Company è presente, ne restituisce il valore
                return ((AssemblyCompanyAttribute)attributes[0]).Company;
            }
        }
        #endregion
    }
}
