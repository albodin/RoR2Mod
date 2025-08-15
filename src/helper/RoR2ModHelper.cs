using System;
using RoR2;
using UnityEngine;

namespace RoR2ModHelper
{
    public class UnityMainThreadDispatcher : MonoBehaviour
    {
        private static volatile UnityMainThreadDispatcher _instance;
        private static readonly System.Collections.Generic.Queue<Action> _actionQueue =
            new System.Collections.Generic.Queue<Action>();
        private static readonly object _lock = new object();

        void Awake()
        {
            if (Application.isBatchMode) return;

            if (_instance != null)
            {
                Destroy(gameObject);
            }
            else
            {
                _instance = this;
                DontDestroyOnLoad(gameObject);
            }
        }

        void Update()
        {
            lock (_lock)
            {
                while (_actionQueue.Count > 0)
                {
                    try
                    {
                        _actionQueue.Dequeue()?.Invoke();
                    }
                    catch (Exception ex)
                    {
                        Debug.LogError($"RoR2ModHelper: Action error: {ex.Message}");
                    }
                }
            }
        }

        public void ExecuteOnMainThread(Action action)
        {
            if (action == null) return;

            lock (_lock)
            {
                _actionQueue.Enqueue(action);
            }
        }

        public static void Enqueue(Action action)
        {
            if (action == null) return;

            if (_instance == null)
            {
                lock (_lock)
                {
                    if (_instance == null)
                    {
                        CreateDispatcher();
                    }
                }
            }

            if (_instance != null)
            {
                _instance.ExecuteOnMainThread(action);
            }
            else
            {
                action();
            }
        }

        private static void CreateDispatcher()
        {
            try
            {
                GameObject dispatcherGO = new GameObject("RoR2ModHelper_MainThreadDispatcher");
                _instance = dispatcherGO.AddComponent<UnityMainThreadDispatcher>();
            }
            catch (Exception ex)
            {
                Debug.LogError($"RoR2ModHelper: Dispatcher creation failed: {ex.Message}");
            }
        }

        public static void CleanupInstance()
        {
            Debug.Log("RoR2ModHelper: CleanupInstance called");

            try
            {
                lock (_lock)
                {
                    Debug.Log($"RoR2ModHelper: Clearing action queue (count: {_actionQueue.Count})");
                    _actionQueue.Clear();

                    Debug.Log($"RoR2ModHelper: Instance check - _instance != null: {_instance != null}");
                    if (_instance != null)
                    {
                        Debug.Log($"RoR2ModHelper: GameObject check - gameObject != null: {_instance.gameObject != null}");
                        if (_instance.gameObject != null)
                        {
                            Debug.Log("RoR2ModHelper: Destroying GameObject...");
                            UnityEngine.Object.Destroy(_instance.gameObject);
                            Debug.Log("RoR2ModHelper: GameObject.Destroy called successfully");
                        }

                        Debug.Log("RoR2ModHelper: Setting _instance to null...");
                        _instance = null;
                        Debug.Log("RoR2ModHelper: _instance set to null");
                    }
                }
                Debug.Log("RoR2ModHelper: CleanupInstance completed successfully");
            }
            catch (System.Exception ex)
            {
                Debug.LogError($"RoR2ModHelper: CleanupInstance exception: {ex.Message}");
                Debug.LogError($"RoR2ModHelper: CleanupInstance stack trace: {ex.StackTrace}");
            }
        }
    }

    public static class SpawnHelper
    {
        public static void SpawnInteractable(string resourcePath, float x, float y, float z)
        {
            if (string.IsNullOrEmpty(resourcePath))
            {
                Debug.LogError($"RoR2ModHelper: Empty resource path provided");
                return;
            }

            UnityMainThreadDispatcher.Enqueue(() =>
            {
                try
                {
                    Debug.Log($"RoR2ModHelper: Attempting to spawn {resourcePath}");

                    SpawnCard spawnCard = LegacyResourcesAPI.Load<SpawnCard>(resourcePath);
                    if (spawnCard == null)
                    {
                        Debug.LogError($"RoR2ModHelper: SpawnCard not found for path: {resourcePath}");
                        return;
                    }

                    if (spawnCard.prefab == null)
                    {
                        Debug.LogError($"RoR2ModHelper: SpawnCard has null prefab: {resourcePath}");
                        return;
                    }

                    var obj = UnityEngine.Object.Instantiate(spawnCard.prefab);
                    if (obj == null)
                    {
                        Debug.LogError($"RoR2ModHelper: Failed to instantiate prefab: {resourcePath}");
                        return;
                    }

                    obj.transform.position = new Vector3(x, y, z);
                    Debug.Log($"RoR2ModHelper: Successfully spawned {resourcePath} at ({x:F2}, {y:F2}, {z:F2})");
                }
                catch (Exception ex)
                {
                    Debug.LogError($"RoR2ModHelper: Spawn failed for {resourcePath}: {ex.Message}");
                    Debug.LogError($"RoR2ModHelper: Stack trace: {ex.StackTrace}");
                }
            });
        }

        public static bool Cleanup()
        {
            Debug.Log("RoR2ModHelper: SpawnHelper.Cleanup called from C++");
            try
            {
                UnityMainThreadDispatcher.CleanupInstance();
                Debug.Log("RoR2ModHelper: SpawnHelper.Cleanup completed successfully");
                return true;
            }
            catch (System.Exception ex)
            {
                Debug.LogError($"RoR2ModHelper: SpawnHelper.Cleanup exception: {ex.Message}");
                Debug.LogError($"RoR2ModHelper: SpawnHelper.Cleanup stack trace: {ex.StackTrace}");
                return false;
            }
        }
    }
}
