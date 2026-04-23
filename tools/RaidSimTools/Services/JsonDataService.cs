using System;
using System.IO;
using System.Text.Json;
using RaidSimTools.Models;

namespace RaidSimTools.Services;

public class JsonDataService
{
    private static readonly JsonSerializerOptions Options = new()
    {
        WriteIndented = true,
        PropertyNameCaseInsensitive = true
    };

    private static readonly string DataDir =
        Environment.GetEnvironmentVariable("RAIDSIM_DATA_DIR")
        ?? Path.Combine(Environment.CurrentDirectory, "src", "data");

    // ── Spells ──────────────────────────────────────────────────────────────

    public List<Spell> LoadSpells()
    {
        var json = File.ReadAllText($"{DataDir}/spells.json");
        return JsonSerializer.Deserialize<SpellFile>(json, Options)?.Spells ?? [];
    }

    public void SaveSpells(List<Spell> spells)
    {
        var json = JsonSerializer.Serialize(new SpellFile { Spells = spells }, Options);
        File.WriteAllText($"{DataDir}/spells.json", json);
    }

    // ── Specs ───────────────────────────────────────────────────────────────

    public List<Spec> LoadSpecs()
    {
        var json = File.ReadAllText($"{DataDir}/specs.json");
        return JsonSerializer.Deserialize<SpecFile>(json, Options)?.Specializations ?? [];
    }

    public void SaveSpecs(List<Spec> specs)
    {
        var json = JsonSerializer.Serialize(new SpecFile { Specializations = specs }, Options);
        File.WriteAllText($"{DataDir}/specs.json", json);
    }

    // ── Players ─────────────────────────────────────────────────────────────

    public List<Player> LoadPlayers()
    {
        var json = File.ReadAllText($"{DataDir}/players.json");
        return JsonSerializer.Deserialize<PlayerFile>(json, Options)?.Players ?? [];
    }

    public void SavePlayers(List<Player> players)
    {
        var json = JsonSerializer.Serialize(new PlayerFile { Players = players }, Options);
        File.WriteAllText($"{DataDir}/players.json", json);
    }

    // ── Bosses ──────────────────────────────────────────────────────────────

    public List<Boss> LoadBosses()
    {
        var json = File.ReadAllText($"{DataDir}/bosses.json");
        return JsonSerializer.Deserialize<BossFile>(json, Options)?.Bosses ?? [];
    }

    public void SaveBosses(List<Boss> bosses)
    {
        var json = JsonSerializer.Serialize(new BossFile { Bosses = bosses }, Options);
        File.WriteAllText($"{DataDir}/bosses.json", json);
    }
}
