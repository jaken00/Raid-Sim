using System.Collections.ObjectModel;
using System.Windows;
using System.Windows.Controls;
using RaidSimTools.Models;
using RaidSimTools.Services;

namespace RaidSimTools;

public partial class MainWindow : Window
{
    private readonly JsonDataService _dataService = new();

    // ObservableCollection tells the DataGrid when items are added/removed
    private ObservableCollection<Spell> _spells = [];

    public MainWindow()
    {
        InitializeComponent();
        RefreshActiveTab();
    }

    private void BtnBrowse_Click(object sender, RoutedEventArgs e)
    {
        // Path is hardcoded — Browse button is unused for now
    }

    private void BtnSave_Click(object sender, RoutedEventArgs e)
    {
        switch (Tabs.SelectedIndex)
        {
            case 0:
                _dataService.SaveSpells(_spells.ToList());
                MessageBox.Show("Spells saved!", "Raid Sim Tools", MessageBoxButton.OK, MessageBoxImage.Information);
                break;
            // TODO: cases 1-3 once you add the other DataGrids
        }
    }

    private void Tabs_SelectionChanged(object sender, SelectionChangedEventArgs e)
    {
        RefreshActiveTab();
    }

    private void RefreshActiveTab()
    {
        switch (Tabs.SelectedIndex)
        {
            case 0:
                _spells = new ObservableCollection<Spell>(_dataService.LoadSpells());
                SpellsGrid.ItemsSource = _spells;
                break;
            // TODO: cases 1-3 once you add the other DataGrids
        }
    }
}
