using System;
using System.Windows.Forms;

namespace CollaborativeTextEditorClient
{
    static class Program
    {
        /// <summary>
        /// Główny punkt wejścia dla aplikacji.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            new Form1().Show();
            Application.Run();
        }
    }
}
