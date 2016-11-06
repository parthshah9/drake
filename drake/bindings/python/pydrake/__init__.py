from . import rbtree
rbtree.RigidBodyTree = rbtree.RigidBodyTree_d

try:
    from autogenerated_path import getDrakePath
except ImportError:
    from insource_path import getDrakePath