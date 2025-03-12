import os
import shutil
import sys

import numpy as np
import bpy
import bmesh
import json


def install_libraries():
    pil_missing = False
    opencv_missing = False
    try:
        import PIL
    except ModuleNotFoundError:
        pil_missing = True
    try:
        import cv2
    except ModuleNotFoundError:
        opencv_missing = True
    if pil_missing or opencv_missing:
        py_exe = sys.executable
        res_path = os.path.realpath(os.path.join(py_exe, "../../lib/site-packages"))
        print("Run the following command as an admin: ")
        print('&"' + py_exe + '" -m pip install pillow opencv-python "--target=' + res_path + '"')


install_libraries()

BOT_ARM_TRANS = [-0.072266 - 0.006897, 0.085937 + 0.009853]
TOP_ARM_TRANS = [0.043945, 0.006836]
RIGHT_LEG_TRANS = -0.014389
BODY_TRANS = [0.0, 0.170266]
LIP_TRANS = [1 / 4 + 1 / 16, -1 / 32]
LEFT_LEG_COLOR = (64 * 256 + 24) * 256 + 126
RIGHT_LEG_COLOR = (42 * 256 + 126) * 256 + 24
BOT_ARM_COLOR = (62 * 256 + 21) * 256 + 211
LIP_COLOR = (21 * 256 + 109) * 256 + 211
TOP_ARM_COLOR = (255 * 256 + 0) * 256 + 0
BODY_COLOR = (21 * 256 + 211) * 256 + 91
HEAD_COLOR = (204 * 256 + 162) * 256 + 20
NEW_GP_UV_MAP = 'unified_gp_uv'
UE5_BONE_HIERARCHY = {'pelvis': '', 'spine_01': 'pelvis', 'spine_02': 'spine_01', 'spine_03': 'spine_02',
                      'spine_04': 'spine_03', 'spine_05': 'spine_04', 'neck_01': 'spine_05', 'neck_02': 'neck_01',
                      'head': 'neck_02', 'clavicle_l': 'spine_05', 'upperarm_l': 'clavicle_l',
                      'lowerarm_l': 'upperarm_l', 'lowerarm_twist_02_l': 'lowerarm_l',
                      'lowerarm_twist_01_l': 'lowerarm_l', 'lowerarm_correctiveRoot_l': 'lowerarm_l',
                      'lowerarm_in_l': 'lowerarm_correctiveRoot_l', 'lowerarm_out_l': 'lowerarm_correctiveRoot_l',
                      'lowerarm_fwd_l': 'lowerarm_correctiveRoot_l', 'lowerarm_bck_l': 'lowerarm_correctiveRoot_l',
                      'hand_l': 'lowerarm_l', 'wrist_inner_l': 'hand_l', 'wrist_outer_l': 'hand_l',
                      'index_metacarpal_l': 'hand_l', 'index_01_l': 'index_metacarpal_l', 'index_02_l': 'index_01_l',
                      'index_03_l': 'index_02_l', 'middle_metacarpal_l': 'hand_l', 'middle_01_l': 'middle_metacarpal_l',
                      'middle_02_l': 'middle_01_l', 'middle_03_l': 'middle_02_l', 'thumb_01_l': 'hand_l',
                      'thumb_02_l': 'thumb_01_l', 'thumb_03_l': 'thumb_02_l', 'pinky_metacarpal_l': 'hand_l',
                      'pinky_01_l': 'pinky_metacarpal_l', 'pinky_02_l': 'pinky_01_l', 'pinky_03_l': 'pinky_02_l',
                      'ring_metacarpal_l': 'hand_l', 'ring_01_l': 'ring_metacarpal_l', 'ring_02_l': 'ring_01_l',
                      'ring_03_l': 'ring_02_l', 'weapon_l': 'hand_l', 'upperarm_twist_01_l': 'upperarm_l',
                      'upperarm_twistCor_01_l': 'upperarm_twist_01_l', 'upperarm_twist_02_l': 'upperarm_l',
                      'upperarm_tricep_l': 'upperarm_twist_02_l', 'upperarm_bicep_l': 'upperarm_twist_02_l',
                      'upperarm_twistCor_02_l': 'upperarm_twist_02_l', 'upperarm_correctiveRoot_l': 'upperarm_l',
                      'upperarm_bck_l': 'upperarm_correctiveRoot_l', 'upperarm_fwd_l': 'upperarm_correctiveRoot_l',
                      'upperarm_in_l': 'upperarm_correctiveRoot_l', 'upperarm_out_l': 'upperarm_correctiveRoot_l',
                      'clavicle_out_l': 'clavicle_l', 'clavicle_scap_l': 'clavicle_l', 'clavicle_r': 'spine_05',
                      'upperarm_r': 'clavicle_r', 'lowerarm_r': 'upperarm_r', 'lowerarm_twist_02_r': 'lowerarm_r',
                      'lowerarm_twist_01_r': 'lowerarm_r', 'lowerarm_correctiveRoot_r': 'lowerarm_r',
                      'lowerarm_out_r': 'lowerarm_correctiveRoot_r', 'lowerarm_in_r': 'lowerarm_correctiveRoot_r',
                      'lowerarm_fwd_r': 'lowerarm_correctiveRoot_r', 'lowerarm_bck_r': 'lowerarm_correctiveRoot_r',
                      'hand_r': 'lowerarm_r', 'wrist_inner_r': 'hand_r', 'wrist_outer_r': 'hand_r',
                      'pinky_metacarpal_r': 'hand_r', 'pinky_01_r': 'pinky_metacarpal_r', 'pinky_02_r': 'pinky_01_r',
                      'pinky_03_r': 'pinky_02_r', 'ring_metacarpal_r': 'hand_r', 'ring_01_r': 'ring_metacarpal_r',
                      'ring_02_r': 'ring_01_r', 'ring_03_r': 'ring_02_r', 'middle_metacarpal_r': 'hand_r',
                      'middle_01_r': 'middle_metacarpal_r', 'middle_02_r': 'middle_01_r', 'middle_03_r': 'middle_02_r',
                      'index_metacarpal_r': 'hand_r', 'index_01_r': 'index_metacarpal_r', 'index_02_r': 'index_01_r',
                      'index_03_r': 'index_02_r', 'thumb_01_r': 'hand_r', 'thumb_02_r': 'thumb_01_r',
                      'thumb_03_r': 'thumb_02_r', 'weapon_r': 'hand_r', 'upperarm_twist_01_r': 'upperarm_r',
                      'upperarm_twistCor_01_r': 'upperarm_twist_01_r', 'upperarm_twist_02_r': 'upperarm_r',
                      'upperarm_tricep_r': 'upperarm_twist_02_r', 'upperarm_bicep_r': 'upperarm_twist_02_r',
                      'upperarm_twistCor_02_r': 'upperarm_twist_02_r', 'upperarm_correctiveRoot_r': 'upperarm_r',
                      'upperarm_bck_r': 'upperarm_correctiveRoot_r', 'upperarm_in_r': 'upperarm_correctiveRoot_r',
                      'upperarm_fwd_r': 'upperarm_correctiveRoot_r', 'upperarm_out_r': 'upperarm_correctiveRoot_r',
                      'clavicle_out_r': 'clavicle_r', 'clavicle_scap_r': 'clavicle_r', 'clavicle_pec_r': 'spine_05',
                      'spine_04_latissimus_l': 'spine_05', 'spine_04_latissimus_r': 'spine_05',
                      'clavicle_pec_l': 'spine_05', 'thigh_r': 'pelvis', 'calf_r': 'thigh_r', 'foot_r': 'calf_r',
                      'ball_r': 'foot_r', 'ankle_fwd_r': 'foot_r', 'ankle_bck_r': 'foot_r', 'calf_twist_02_r': 'calf_r',
                      'calf_twistCor_02_r': 'calf_twist_02_r', 'calf_twist_01_r': 'calf_r',
                      'calf_correctiveRoot_r': 'calf_r', 'calf_kneeBack_r': 'calf_correctiveRoot_r',
                      'calf_knee_r': 'calf_correctiveRoot_r', 'thigh_twist_01_r': 'thigh_r',
                      'thigh_twistCor_01_r': 'thigh_twist_01_r', 'thigh_twist_02_r': 'thigh_r',
                      'thigh_twistCor_02_r': 'thigh_twist_02_r', 'thigh_correctiveRoot_r': 'thigh_r',
                      'thigh_fwd_r': 'thigh_correctiveRoot_r', 'thigh_bck_r': 'thigh_correctiveRoot_r',
                      'thigh_out_r': 'thigh_correctiveRoot_r', 'thigh_in_r': 'thigh_correctiveRoot_r',
                      'thigh_bck_lwr_r': 'thigh_correctiveRoot_r', 'thigh_fwd_lwr_r': 'thigh_correctiveRoot_r',
                      'thigh_l': 'pelvis', 'calf_l': 'thigh_l', 'foot_l': 'calf_l', 'ball_l': 'foot_l',
                      'ankle_bck_l': 'foot_l', 'ankle_fwd_l': 'foot_l', 'calf_twist_02_l': 'calf_l',
                      'calf_twistCor_02_l': 'calf_twist_02_l', 'calf_twist_01_l': 'calf_l',
                      'calf_correctiveRoot_l': 'calf_l', 'calf_kneeBack_l': 'calf_correctiveRoot_l',
                      'calf_knee_l': 'calf_correctiveRoot_l', 'thigh_twist_01_l': 'thigh_l',
                      'thigh_twistCor_01_l': 'thigh_twist_01_l', 'thigh_twist_02_l': 'thigh_l',
                      'thigh_twistCor_02_l': 'thigh_twist_02_l', 'thigh_correctiveRoot_l': 'thigh_l',
                      'thigh_bck_l': 'thigh_correctiveRoot_l', 'thigh_fwd_l': 'thigh_correctiveRoot_l',
                      'thigh_out_l': 'thigh_correctiveRoot_l', 'thigh_bck_lwr_l': 'thigh_correctiveRoot_l',
                      'thigh_in_l': 'thigh_correctiveRoot_l', 'thigh_fwd_lwr_l': 'thigh_correctiveRoot_l',
                      'interaction': '', 'center_of_mass': '', 'IK_PT_Foot_L': '', 'IK_PT_Foot_R': '',
                      'IK_PT_Hand_L': '', 'IK_PT_Hand_R': '', 'ControllersPanel': '', 'LFootIKFK': 'ControllersPanel',
                      'RFootIKFK': 'ControllersPanel', 'LHandIKFK': 'ControllersPanel', 'RHandIKFK': 'ControllersPanel',
                      'BalanceAutoManual': 'ControllersPanel', 'Base': '', 'FootRoot': 'Base', 'IK_Foot_L': 'FootRoot',
                      'IK_Foot_Rotation_L': 'IK_Foot_L', 'Foot_Offset_L': 'IK_Foot_Rotation_L',
                      'IK_Foot_Solver_L': 'IK_Foot_Rotation_L', 'IK_LegLength_Target_L': 'IK_Foot_Solver_L',
                      'IK_Foot_R': 'FootRoot', 'IK_Foot_Rotation_R': 'IK_Foot_R', 'Foot_Offset_R': 'IK_Foot_Rotation_R',
                      'IK_Foot_Solver_R': 'IK_Foot_Rotation_R', 'IK_LegLength_Target_R': 'IK_Foot_Solver_R',
                      'HandRoot': 'Base', 'IK_Hand_L': 'HandRoot', 'IK_Hand_Solver_L': 'IK_Hand_L',
                      'IK_ArmLength_Target_L': 'IK_Hand_Solver_L', 'IK_Hand_R': 'HandRoot',
                      'IK_Hand_Solver_R': 'IK_Hand_R', 'IK_ArmLength_Target_R': 'IK_Hand_Solver_R',
                      'TorsoRotation': 'Base', 'PelvisControl': 'TorsoRotation', 'Pelvis': 'PelvisControl',
                      'Spine_01': 'Pelvis', 'Spine_02': 'Spine_01', 'Spine_03': 'Spine_02', 'Spine_04': 'Spine_03',
                      'Spine_05': 'Spine_04', 'Neck_01': 'Spine_05', 'Neck_02': 'Neck_01', 'Head': 'Neck_02',
                      'Clavicle_L': 'Spine_05', 'UpperArm_L': 'Clavicle_L', 'LowerArm_L': 'UpperArm_L',
                      'Hand_L': 'LowerArm_L', 'Ring_L': 'Hand_L', 'Index_Metacarpal_L': 'Hand_L',
                      'Index_01_L': 'Index_Metacarpal_L', 'Index_02_L': 'Index_01_L', 'Index_03_L': 'Index_02_L',
                      'Middle_Metacarpal_L': 'Hand_L', 'Middle_01_L': 'Middle_Metacarpal_L',
                      'Middle_02_L': 'Middle_01_L', 'Middle_03_L': 'Middle_02_L', 'Thumb_Metacarpal_L': 'Hand_L',
                      'Thumb_01_L': 'Thumb_Metacarpal_L', 'Thumb_02_L': 'Thumb_01_L', 'Ring_Metacarpal_L': 'Hand_L',
                      'Ring_01_L': 'Ring_Metacarpal_L', 'Ring_02_L': 'Ring_01_L', 'Ring_03_L': 'Ring_02_L',
                      'Pinky_Metacarpal_L': 'Hand_L', 'Pinky_01_L': 'Pinky_Metacarpal_L', 'Pinky_02_L': 'Pinky_01_L',
                      'Pinky_03_L': 'Pinky_02_L', 'Fingers_L': 'Hand_L', 'Thumb_L': 'Hand_L', 'Index_L': 'Hand_L',
                      'Middle_L': 'Hand_L', 'Pinky_L': 'Hand_L', 'Weapon_Socket_L': 'Hand_L', 'Clavicle_R': 'Spine_05',
                      'UpperArm_R': 'Clavicle_R', 'LowerArm_R': 'UpperArm_R', 'Hand_R': 'LowerArm_R',
                      'Index_Metacarpal_R': 'Hand_R', 'Index_01_R': 'Index_Metacarpal_R', 'Index_02_R': 'Index_01_R',
                      'Index_03_R': 'Index_02_R', 'Middle_Metacarpal_R': 'Hand_R', 'Middle_01_R': 'Middle_Metacarpal_R',
                      'Middle_02_R': 'Middle_01_R', 'Middle_03_R': 'Middle_02_R', 'Thumb_Metacarpal_R': 'Hand_R',
                      'Thumb_01_R': 'Thumb_Metacarpal_R', 'Thumb_02_R': 'Thumb_01_R', 'Ring_Metacarpal_R': 'Hand_R',
                      'Ring_01_R': 'Ring_Metacarpal_R', 'Ring_02_R': 'Ring_01_R', 'Ring_03_R': 'Ring_02_R',
                      'Pinky_Metacarpal_R': 'Hand_R', 'Pinky_01_R': 'Pinky_Metacarpal_R', 'Pinky_02_R': 'Pinky_01_R',
                      'Pinky_03_R': 'Pinky_02_R', 'Ring_R': 'Hand_R', 'Pinky_R': 'Hand_R', 'Thumb_R': 'Hand_R',
                      'Index_R': 'Hand_R', 'Fingers_R': 'Hand_R', 'Middle_R': 'Hand_R', 'Weapon_Socket_R': 'Hand_R',
                      'IK_ArmLenght_L': 'Spine_05', 'Shoulder_Target_L': 'IK_ArmLenght_L', 'IK_ArmLength_R': 'Spine_05',
                      'Shoulder_Target_R': 'IK_ArmLength_R', 'IK_LegLenght_L': 'Spine_05', 'IK_LegLenght_R': 'Spine_05',
                      'Tigh_L': 'Pelvis', 'Calf_L': 'Tigh_L', 'Foot_L': 'Calf_L', 'Ball_L': 'Foot_L',
                      'Tigh_R': 'Pelvis', 'Calf_R': 'Tigh_R', 'Foot_R': 'Calf_R', 'Ball_R': 'Foot_R',
                      'PelvisHeightOffset': 'PelvisControl', 'TorsoControl': 'PelvisControl',
                      'ChestControl': 'TorsoControl', 'HeadControl': 'ChestControl', 'ChestRotation': 'TorsoRotation',
                      'AimTarget': '', 'ik_foot_root': '', 'ik_foot_l': 'ik_foot_root', 'ik_foot_r': 'ik_foot_root',
                      'ik_hand_root': '', 'ik_hand_gun': 'ik_hand_root', 'ik_hand_l': 'ik_hand_gun',
                      'ik_hand_r': 'ik_hand_gun'}


def select_bone(bone):
    bone.select = True
    bone.select_head = True
    bone.select_tail = True


def find_body_rig():
    o = bpy.data.objects[0]
    while o.parent is not None:
        o = o.parent
    return o


class DazOptimizer:

    def __init__(self, workdir=None, name=None):
        if workdir is None:
            workdir = os.path.dirname(bpy.data.filepath)
        if name is None:
            name = os.path.basename(bpy.data.filepath)
            name = name[:name.rindex('.')]
        self.name = name
        self.workdir = workdir
        self.body_mesh = None
        self.body_rig = None

    def gold_palace_dir(self):
        return os.path.join(self.workdir, "textures/original/meipex/m_goldenpalace/g9")

    def us_mask_path(self):
        return os.path.join(self.workdir, "uv_region_mask.png")

    def textures_dir(self):
        return os.path.join(self.workdir, "textures")

    def genesis_dir(self):
        return os.path.join(self.workdir, "textures/original/daz/characters/genesis9")

    def get_uv_mask(self):
        from PIL import Image
        uv_region_mask = np.array(Image.open(self.us_mask_path()), dtype=np.uint32)
        uv_region_mask = (uv_region_mask[:, :, 0] * 256 + uv_region_mask[:, :, 1]) * 256 + uv_region_mask[:, :, 2]
        return uv_region_mask

    def find_body(self):
        self.body_rig = find_body_rig()
        self.body_mesh = bpy.data.objects[self.body_rig.name + ' Mesh']

    def get_body_mesh(self):
        if self.body_mesh is None:
            self.find_body()
        return self.body_mesh

    def get_body_rig(self):
        if self.body_rig is None:
            self.find_body()
        return self.body_rig

    def get_eyes_mesh(self):
        return bpy.data.objects['Genesis 9 Eyes Mesh']

    def get_base_uv_layer(self, layer_name='Base Multi UDIM'):
        return self.get_body_mesh().data.uv_layers[layer_name]

    def get_base_uv_layer_np(self, layer_name='Base Multi UDIM'):
        return np.array([v.uv for v in self.get_base_uv_layer(layer_name=layer_name).data])

    def get_base_uv_layer_selection_np(self):
        return np.array([v.select for v in self.get_base_uv_layer().data], dtype=bool)

    def update_base_uv_layer(self, base_layer_np: np.ndarray):
        for v, new_uv in zip(self.get_base_uv_layer().data, base_layer_np):
            v.uv = new_uv

    def optimize_eyes(self):
        EYES_M = self.get_eyes_mesh()
        bpy.ops.object.select_all(action='DESELECT')
        EYES_M.select_set(True)
        bpy.context.view_layer.objects.active = EYES_M

        # uv_layer = EYES_M.data.uv_layers.active
        # uvs = np.array([v.uv for v in uv_layer.data], dtype=bool)
        # uvs[:, y] < 0.5
        bpy.ops.object.mode_set(mode='EDIT')
        bpy.context.scene.tool_settings.use_uv_select_sync = False
        bpy.ops.uv.select_all(action='DESELECT')
        bpy.ops.mesh.select_all(action='DESELECT')

        me = bpy.context.object.data
        bm = bmesh.from_edit_mesh(me)
        uv_layer = bm.loops.layers.uv.verify()

        for v in bm.verts:
            v.select = False

        for face in bm.faces:
            for loop in face.loops:
                loop_uv = loop[uv_layer]
                uv = np.array(loop_uv.uv)
                dist = np.linalg.norm(uv % 0.5 - 0.25)
                if uv[1] < 0.5 or dist > 0.24 or dist < 0.038:
                    loop.vert.select = True
        for v in bm.verts:
            if v.select:
                bm.verts.remove(v)

        def select_loop(center):
            for face in bm.faces:
                for loop in face.loops:
                    loop_uv = loop[uv_layer]
                    uv = np.array(loop_uv.uv)
                    dist = np.linalg.norm(uv - center)
                    loop.vert.select_set(dist < 0.044)

        bmesh.update_edit_mesh(me)

        select_loop([0.25, 0.75])
        bpy.ops.mesh.edge_face_add()
        select_loop([0.75, 0.75])
        bpy.ops.mesh.edge_face_add()

        bmesh.update_edit_mesh(me)

        bpy.ops.object.mode_set(mode='OBJECT')
        #

    def simplify_materials(self):
        BODY_M = self.get_body_mesh()

        def collect_all_before(node, outputs):
            if node not in outputs:
                outputs.add(node)
                for input_socket in node.inputs:
                    for link in input_socket.links:
                        collect_all_before(link.from_node, outputs)
            return outputs

        def delete_all_before(node_tree, node):
            for node in collect_all_before(node, set()):
                node_tree.nodes.remove(node)

        def backwards_search_for(node, t: type, outputs):
            if node not in outputs:
                if isinstance(node, t):
                    outputs.add(node)
                for i in node.inputs:
                    from_socket_backwards_search_for(i, t, outputs)
            return outputs

        def from_socket_backwards_search_for(input_socket, t: type, outputs):
            for link in input_socket.links:
                backwards_search_for(link.from_node, t, outputs)
            return outputs

        def find_by_type(node_tree, t: type):
            for node in node_tree.nodes:
                if isinstance(node, t):
                    return node

        all_filepaths: {str: {str: [bpy.types.Image]}} = {}
        for mat in BODY_M.data.materials:
            output_node = find_by_type(mat.node_tree, bpy.types.ShaderNodeOutputMaterial)
            body_part = mat.name.rstrip('0123456789-_')
            body_part_filepaths = all_filepaths[body_part] = {'Base Color':set(), 'Roughness':set(), 'Normal':set()}
            if output_node is not None:
                for bsdf in from_socket_backwards_search_for(output_node.inputs['Surface'], (bpy.types.ShaderNodeBsdfPrincipled, bpy.types.ShaderNodeGroup), set()):
                    if isinstance(bsdf, bpy.types.ShaderNodeBsdfPrincipled):
                        for channel in ['Base Color', 'Roughness', 'Normal']:
                            for img_node in from_socket_backwards_search_for(bsdf.inputs[channel],  bpy.types.ShaderNodeTexImage, set()):
                                body_part_filepaths[channel].add(img_node.image)
                    elif bsdf.node_tree.name == 'DAZ Dual Lobe PBR':
                        for img_node in from_socket_backwards_search_for(bsdf.inputs['Roughness 1'], bpy.types.ShaderNodeTexImage, set()):
                            body_part_filepaths['Roughness'].add(img_node.image)
                        for img_node in from_socket_backwards_search_for(bsdf.inputs['Roughness 2'], bpy.types.ShaderNodeTexImage, set()):
                            body_part_filepaths['Roughness'].add(img_node.image)
                        for img_node in from_socket_backwards_search_for(bsdf.inputs['Normal'], bpy.types.ShaderNodeTexImage, set()):
                            body_part_filepaths['Normal'].add(img_node.image)
        for body_part_filepaths in all_filepaths.values():
            occurrences = {}
            for channel in body_part_filepaths.values():
                for image in channel:
                    if image not in occurrences:
                        occurrences[image] = 1
                    else:
                        occurrences[image] += 1
            for channel in body_part_filepaths:
                body_part_filepaths[channel] = list(sorted(body_part_filepaths[channel], key=lambda x: occurrences[x]))

        print(json.dumps({k:{k2:[v3.filepath for v3 in v2] for k2, v2 in v.items()} for k, v in all_filepaths.items()}, indent=2))

        def gen_simple_material(node_tree, filepaths, output_socket, shift_x=0, uvs=None):
            ns = node_tree.nodes
            ls = node_tree.links
            bsdf_node = ns.new('ShaderNodeBsdfPrincipled')
            bsdf_node.location = (shift_x, 0)
            if isinstance(uvs, str):
                uv_node = ns.new('ShaderNodeUVMap')
                uv_node.location = (-900+shift_x,0)
                uv_node.uv_map = uvs
                uvs = uv_node
            if isinstance(uvs, bpy.types.ShaderNodeUVMap):
                uvs = uvs.outputs['UV']
            for idx, channel in enumerate(['Base Color', 'Roughness', 'Normal']):
                tex_node = ns.new('ShaderNodeTexImage')
                tex_node.location = (-600+shift_x, -(idx-1) * 300)
                tex_node.image = filepaths[channel][0]
                if uvs is not None:
                    ls.new(tex_node.inputs['Vector'], uvs)
                if channel == 'Normal':
                    norm_map_node = ns.new('ShaderNodeNormalMap')
                    norm_map_node.location = (-200+shift_x, -idx * 200)
                    ls.new(bsdf_node.inputs[channel], norm_map_node.outputs['Normal'])
                    ls.new(norm_map_node.inputs['Color'], tex_node.outputs['Color'])
                else:
                    ls.new(bsdf_node.inputs[channel], tex_node.outputs['Color'])
            ls.new(output_socket, bsdf_node.outputs['BSDF'])

        for mat in BODY_M.data.materials:
            body_part = mat.name.rstrip('0123456789-_')
            body_part_filepaths = all_filepaths[body_part]
            mat.node_tree.nodes.clear()
            output_node = mat.node_tree.nodes.new('ShaderNodeOutputMaterial')
            output_node.location = (400, 0)
            gen_simple_material(mat.node_tree, body_part_filepaths, output_node.inputs['Surface'])

        body_part_filepaths = all_filepaths['Body']
        if 'GoldenPalace_G9' in bpy.data.objects:
            GOLD_PAL_M = bpy.data.objects['GoldenPalace_G9 Mesh']
            for mat in GOLD_PAL_M.data.materials:
                output_node = find_by_type(mat.node_tree, bpy.types.ShaderNodeOutputMaterial)
                tail = output_node.inputs['Surface'].links[0].from_node
                while isinstance(tail, bpy.types.ShaderNodeGroup) and tail.node_tree.name.startswith('GoldenPalaceG9_Shell_'):
                    output_node = tail
                    tail = output_node.inputs['BSDF'].links[0].from_node
                out_socket = output_node.inputs['BSDF']
                delete_all_before(mat.node_tree, out_socket.links[0].from_node)
                gen_simple_material(mat.node_tree, body_part_filepaths, out_socket, shift_x=output_node.location[0]-300,uvs='Default UVs')

    def concat_textures(self):
        from PIL import Image
        import re
        import cv2
        gp_baked_path = os.path.join(self.workdir, self.name + '_gp_baked.png')
        if 'GP_Baked' in bpy.data.images:
            bpy.data.images['GP_Baked'].save(filepath=gp_baked_path)
        name_pattern = re.compile(r"head|eye(s|lashes|_sclera|_iris)?|legs|nails|body|arms|mouth")
        map_type_pattern = re.compile(r"(D|NM|R|SSS|C)(?=[^a-z])|\bnm\b")

        img_file_paths = {}
        gp_map_types = {
            "Roughness": "R",
            "NormalMap": "NM",
            "Speculairty": "SSS",
            "Color": "D",
        }
        for root, dirs, files in os.walk(self.textures_dir()):
            for file_name in files:
                body_part = None
                map_type = None
                if file_name.endswith(".jpg") or file_name.endswith(".png"):
                    if file_name.startswith("G9GP_"):
                        map_type = file_name[len("G9GP_"):-len(".jpg")]
                        map_type = gp_map_types.get(map_type)
                        body_part = "G9GP"
                    else:
                        match = name_pattern.search(file_name.lower())
                        if match:
                            body_part = match.group()
                        match = map_type_pattern.search(file_name)
                        if match:
                            map_type = match.group()
                    if body_part is not None and map_type is not None:
                        file_path = os.path.join(root, file_name)
                        if body_part not in img_file_paths:
                            img_file_paths[body_part] = {}
                        img_file_paths[body_part][map_type] = file_path

        if os.path.exists(gp_baked_path):
            img_file_paths["G9GP"]["D"] = gp_baked_path
        uv_region_mask = self.get_uv_mask()
        MASK_TILE_SIZE = uv_region_mask.shape[0] // 2
        arms_region_mask = uv_region_mask[:MASK_TILE_SIZE, MASK_TILE_SIZE:]
        body_region_mask = uv_region_mask[MASK_TILE_SIZE:, MASK_TILE_SIZE:]
        legs_region_mask = uv_region_mask[:MASK_TILE_SIZE, :MASK_TILE_SIZE]
        head_region_mask = uv_region_mask[MASK_TILE_SIZE:, :MASK_TILE_SIZE]

        print(json.dumps(img_file_paths, indent=4))
        # from matplotlib import pyplot as plt
        for map_type in ["D", "R", "NM", "SSS"]:
            print("map_type=", map_type)
            head_file: str = img_file_paths['head'][map_type]
            _, extension = head_file.rsplit('.', maxsplit=1)
            head_tile = Image.open(head_file)
            body_tile = Image.open(img_file_paths['body'][map_type])
            arms_tile = Image.open(img_file_paths['arms'][map_type])
            legs_tile = Image.open(img_file_paths['legs'][map_type])

            head_tile = np.array(head_tile)
            body_tile = np.array(body_tile)
            arms_tile = np.array(arms_tile)
            legs_tile = np.array(legs_tile)

            s = legs_tile.shape[0]
            s2 = s * 2
            if ('eyes_iris' in img_file_paths and 'eyes_sclera' in img_file_paths
                    and map_type in img_file_paths['eyes_iris'] and map_type in img_file_paths['eyes_sclera']):
                iris = Image.open(img_file_paths['eyes_iris'][map_type])
                sclera = Image.open(img_file_paths['eyes_sclera'][map_type])
                iris = np.array(iris)
                iris_rgb = iris[:, :, :3]
                iris_a = iris[:, :, 3]
                if iris_a.dtype.kind != 'f':
                    iris_a = iris_a / 255
                sclera = np.array(sclera)
                sclera_h, sclera_w = sclera.shape[:2]
                sclera_h = sclera_h // 2
                sclera = sclera[:sclera_h]
                iris_h, iris_w = s // 8, s // 4
                eyes = iris_rgb.T * iris_a.T + sclera.T * (1 - iris_a.T)
                eyes = eyes.T
                eyes = cv2.resize(eyes, [iris_w, iris_h])

            else:
                eyes = None

            if map_type != "NM":
                nails_tile = Image.open(img_file_paths['nails'][map_type])
                nails_tile = np.array(nails_tile)
                nails_tile_size = s2 // 8
                if map_type == "R":
                    nails_tile = np.average(nails_tile, axis=2)
                    nails_tile = nails_tile.astype(legs_tile.dtype)
                nails_tile = cv2.resize(nails_tile, (nails_tile_size, nails_tile_size))

            else:
                nails_tile = None
            if "G9GP" in img_file_paths and map_type in img_file_paths["G9GP"]:
                gp_tile = Image.open(img_file_paths['G9GP'][map_type])
                gp_tile = np.array(gp_tile)
                gp_tile_size = s2 * 14 // 64
                if map_type == "R":
                    gp_tile = np.average(gp_tile, axis=2)
                    gp_tile = gp_tile.astype(legs_tile.dtype)
                gp_tile = cv2.resize(gp_tile, (gp_tile_size, gp_tile_size))
            else:
                gp_tile = None

            def shift_img(img: np.ndarray, y0, y1, x0, x1, mask: np.ndarray, translation: [float, float], hflip=False):
                shape = [s2, s2, legs_tile.shape[2]] if len(legs_tile.shape) > 2 else [s2, s2]
                new_img = np.zeros(shape, dtype=legs_tile.dtype)
                if hflip:
                    mask = np.flipud(mask)
                    img = np.flipud(img)
                new_img[y0:y1, x0:x1][mask] = img[mask]
                x, y = np.int32(np.array(translation) * s2)
                if x != 0 or y != 0:
                    new_img = np.roll(new_img, [-y, x], axis=[0, 1])
                return new_img

            # Textures are concatenated as follows:
            #   Legs | Arms
            #  ------+-----
            #   Head | Body
            packed = shift_img(arms_tile, 0, s, s, s2, arms_region_mask == BOT_ARM_COLOR, BOT_ARM_TRANS)
            packed += shift_img(arms_tile, 0, s, s, s2, arms_region_mask == TOP_ARM_COLOR, TOP_ARM_TRANS)
            packed += shift_img(legs_tile, 0, s, 0, s, legs_region_mask == LEFT_LEG_COLOR, [0, 0])
            packed += shift_img(legs_tile, 0, s, 0, s, legs_region_mask == RIGHT_LEG_COLOR, [RIGHT_LEG_TRANS, 0], True)
            packed += shift_img(body_tile, s, s2, s, s2, body_region_mask == BODY_COLOR, BODY_TRANS)
            packed += shift_img(head_tile, s, s2, 0, s, head_region_mask == LIP_COLOR, LIP_TRANS)
            # packed += shift_img(head, s, s2, s, s2, head_region_mask == HEAD_COLOR, [0.008526, 0.019377])
            packed[s:, :s] = head_tile
            if nails_tile is not None:
                packed[s2 - nails_tile_size:s2, s:s + nails_tile_size] = nails_tile
            if gp_tile is not None:
                packed[s2 - gp_tile_size:s2, s2 - gp_tile_size:s2] += gp_tile
            if eyes is not None:
                packed[s2 - s // 4 - s // 8:s2 - s // 4, s + s // 4:s + s // 2] = eyes

            # packed[:s, :s] = legs_tile
            # packed[s:, s:] = body_tile
            # packed[:s, s:] = arms_tile
            packed = Image.fromarray(packed)
            packed.save(os.path.join(self.workdir, self.name + '_' + map_type + '.png'))
            # plt.imshow(packed)
            # plt.show()

    def merge_geografts(self):
        BODY_M = self.get_body_mesh()
        BODY_RIG = self.get_body_rig()

        # merge meshes
        bpy.ops.object.select_all(action='DESELECT')
        GOLD_PAL_RIG = None
        if 'GoldenPalace_G9' in bpy.data.objects:
            GOLD_PAL_M = bpy.data.objects['GoldenPalace_G9 Mesh']
            GOLD_PAL_RIG = bpy.data.objects['GoldenPalace_G9']
            GOLD_PAL_M.select_set(True)
        BREASTACULAR_RIG = None
        if 'BreastacularG9' in bpy.data.objects:
            BREASTACULAR_M = bpy.data.objects['BreastacularG9 Mesh']
            BREASTACULAR_RIG = bpy.data.objects['BreastacularG9']
            BREASTACULAR_M.select_set(True)
        BODY_M.select_set(True)
        bpy.context.view_layer.objects.active = BODY_M
        bpy.ops.daz.merge_geografts()

        # merge bones
        if GOLD_PAL_RIG is not None:
            self.merge_two_rigs(BODY_RIG, GOLD_PAL_RIG)
        if BREASTACULAR_RIG is not None:
            self.merge_two_rigs(BODY_RIG, BREASTACULAR_RIG)

    def merge_two_rigs(self, original, addon):
        bpy.ops.object.select_all(action='DESELECT')
        addon.select_set(True)
        bpy.context.view_layer.objects.active = original
        bpy.ops.object.mode_set(mode='OBJECT')

        deform_bones = {bone.name for bone in addon.data.bones if bone.use_deform}
        parents = {bone.name: bone.parent.name for bone in addon.data.bones if bone.parent is not None}

        bpy.ops.object.mode_set(mode='EDIT')
        duplicates = [bone for bone in addon.data.edit_bones if bone.name in original.data.bones]
        for dup in duplicates:
            addon.data.edit_bones.remove(dup)

        addon.select_set(True)
        original.select_set(True)
        bpy.context.view_layer.objects.active = original
        bpy.ops.object.mode_set(mode='OBJECT')
        bpy.ops.object.join()
        for bone in original.data.bones:
            if bone.name in deform_bones:
                bone.use_deform = True
        bpy.ops.object.mode_set(mode='EDIT')
        for bone in original.data.edit_bones:
            parent = parents.get(bone.name)
            if parent is not None:
                bone.parent = original.data.edit_bones[parent]
        bpy.ops.object.mode_set(mode='OBJECT')

    def merge_eyes(self):

        EYES_M = bpy.data.objects['Genesis 9 Eyes Mesh']
        BODY_M = self.get_body_mesh()
        EYES_RIG = bpy.data.objects['Genesis 9 Eyes']
        BODY_RIG = self.get_body_rig()

        uv_map_name = EYES_M.data.uv_layers.active.name

        # merge meshes
        bpy.ops.object.select_all(action='DESELECT')
        EYES_M.select_set(True)
        BODY_M.select_set(True)
        bpy.context.view_layer.objects.active = BODY_M
        bpy.ops.object.join()

        # merge UV maps
        eyes_layer = BODY_M.data.uv_layers[uv_map_name]
        eyes_layer_np = np.array([v.uv for v in eyes_layer.data])
        is_eye = np.all(eyes_layer_np > 0, axis=1)
        base_layer_np = self.get_base_uv_layer_np()
        base_layer_np[is_eye] = eyes_layer_np[is_eye] + [5, 0]
        self.update_base_uv_layer(base_layer_np)
        BODY_M.data.uv_layers.remove(eyes_layer)

        # merge bones
        self.merge_two_rigs(BODY_RIG, EYES_RIG)

    def merge_mouth(self):

        MOUTH_M = bpy.data.objects['Genesis 9 Mouth Mesh']
        BODY_M = self.get_body_mesh()
        MOUTH_RIG = bpy.data.objects['Genesis 9 Mouth']
        BODY_RIG = self.get_body_rig()

        uv_map_name = MOUTH_M.data.uv_layers.active.name

        # merge meshes
        bpy.ops.object.select_all(action='DESELECT')
        MOUTH_M.select_set(True)
        BODY_M.select_set(True)
        bpy.context.view_layer.objects.active = BODY_M
        bpy.ops.object.join()

        # merge UV maps
        mouth_layer = BODY_M.data.uv_layers[uv_map_name]
        mouth_layer_np = np.array([v.uv for v in mouth_layer.data])
        is_mouth = np.all(mouth_layer_np > 0, axis=1)
        base_layer_np = self.get_base_uv_layer_np()
        base_layer_np[is_mouth] = mouth_layer_np[is_mouth] + [6, 0]
        self.update_base_uv_layer(base_layer_np)
        BODY_M.data.uv_layers.remove(mouth_layer)

        # merge bones
        self.merge_two_rigs(BODY_RIG, MOUTH_RIG)

    def pack_uvs(self):

        # ========= Concat UVs =========
        BODY_M = self.get_body_mesh()
        # pack UVs
        bpy.ops.object.select_all(action='DESELECT')
        BODY_M.select_set(True)
        bpy.context.view_layer.objects.active = BODY_M
        bpy.ops.object.mode_set(mode='OBJECT')
        base_layer_np = self.get_base_uv_layer_np()
        is_nails = np.logical_and(4 < base_layer_np[:, 0], base_layer_np[:, 0] < 5)
        is_eyes = np.logical_and(5 < base_layer_np[:, 0], base_layer_np[:, 0] < 6)
        is_mouth = np.logical_and(6 < base_layer_np[:, 0], base_layer_np[:, 0] < 7)
        base_layer_np *= 0.5
        out_of_bounds = base_layer_np[:, 0] > 1
        base_layer_np[out_of_bounds] += [-1, 0.5]
        gp_layer = BODY_M.data.uv_layers['Golden Palace']
        gp_layer_np = np.array([v.uv for v in gp_layer.data])
        is_gp = np.all(gp_layer_np > 0, axis=1)

        uv_mask = self.get_uv_mask()
        uv_mask_size = uv_mask.shape[0]

        def get_pixel_class():
            pixel_coords = np.copy(base_layer_np)
            pixel_coords[:, 1] = 1 - pixel_coords[:, 1]
            pixel_coords = (pixel_coords * uv_mask_size).clip(0, uv_mask_size - 1)
            pixel_coords = np.int32(pixel_coords)
            pix_cls = uv_mask[pixel_coords[:, 1], pixel_coords[:, 0]]
            return pix_cls

        pixel_class = get_pixel_class()

        base_layer_np[pixel_class == BOT_ARM_COLOR] += BOT_ARM_TRANS
        base_layer_np[pixel_class == TOP_ARM_COLOR] += TOP_ARM_TRANS
        base_layer_np[pixel_class == RIGHT_LEG_COLOR, 1] = 1.5 - base_layer_np[pixel_class == RIGHT_LEG_COLOR, 1]
        base_layer_np[pixel_class == RIGHT_LEG_COLOR, 0] += RIGHT_LEG_TRANS
        base_layer_np[pixel_class == BODY_COLOR] += BODY_TRANS
        base_layer_np[is_nails] = base_layer_np[is_nails] / 4 + [0.25, -0.5 / 4]
        base_layer_np[is_eyes] = base_layer_np[is_eyes] / 4 + [-6 / 16 + 0.5 + 1 / 8, - 0.5 / 8]
        base_layer_np[is_gp] = gp_layer_np[is_gp] * 14 / 64 + [(64 - 14) / 64, 0]
        base_layer_np[is_mouth] = base_layer_np[is_mouth] / 4 + [1 / 8, -0.5 / 4]
        self.update_base_uv_layer(base_layer_np)

    def separate_lip_uvs(self):
        BODY_M = self.get_body_mesh()
        # pack UVs
        bpy.ops.object.select_all(action='DESELECT')
        BODY_M.select_set(True)
        bpy.context.view_layer.objects.active = BODY_M
        bpy.ops.object.mode_set(mode='EDIT')

        uv_mask = self.get_uv_mask()
        uv_mask_size = uv_mask.shape[0]

        bpy.context.scene.tool_settings.use_uv_select_sync = False
        bpy.ops.uv.select_all(action='DESELECT')
        bpy.ops.mesh.select_all(action='DESELECT')

        me = bpy.context.object.data
        bm = bmesh.from_edit_mesh(me)
        uv_layer = bm.loops.layers.uv.verify()

        # for v in bm.verts:
        #    v.select_set(False)

        for face in bm.faces:
            full_loop = True
            for loop in face.loops:
                loop_uv = loop[uv_layer]
                uv = np.array(loop_uv.uv)
                uv[1] = 1 - uv[1]
                pixel_coord = (uv * uv_mask_size).clip(0, uv_mask_size - 1)
                pixel_coord = np.int32(pixel_coord)
                pixel_cls = uv_mask[pixel_coord[1], pixel_coord[0]]
                matched = pixel_cls == LIP_COLOR
                # loop_uv.select = matched
                full_loop = full_loop and matched
                # if matched:
                #    loop.vert.select_set(True)
            face.select_set(full_loop)

        # bm.select_mode = {'VERT', 'EDGE', 'FACE'}
        bm.select_flush_mode()
        # bpy.context.tool_settings.mesh_select_mode = (False, False, True)
        bpy.ops.uv.select_all(action='SELECT')
        bpy.ops.mesh.select_all(action='SELECT')
        bpy.ops.uv.select_split()

        bpy.ops.object.mode_set(mode='OBJECT')
        #  def separate_lips(self):
        base_layer_np = self.get_base_uv_layer_np()
        # pixel_class = get_pixel_class()
        selection = self.get_base_uv_layer_selection_np()
        base_layer_np[selection] = base_layer_np[selection] + LIP_TRANS
        self.update_base_uv_layer(base_layer_np)
        # += [0.043945, 0.006836] # top arm
        # += [-0.072266 , 0.085937] # obttom arm
        # += [0.008526, 0.019377] # torso
        # *= 0.25# nails
        # -= 0.5 # nails

    def subdivide_breast_bones(self):

        BODY_RIG = self.get_body_rig()
        bpy.ops.object.select_all(action='DESELECT')
        BODY_RIG.select_set(True)
        bpy.context.view_layer.objects.active = BODY_RIG
        bpy.ops.object.mode_set(mode='EDIT')
        bpy.ops.armature.select_all(action='DESELECT')
        cuts = 2
        for bone_name in ['r_pectoral', 'l_pectoral']:
            bone = BODY_RIG.data.edit_bones[bone_name]
            select_bone(bone)
        bpy.ops.armature.subdivide(number_cuts=cuts)
        l_pec_groups = []
        r_pec_groups = []
        pectorals = [('r_pectoral', r_pec_groups), ('l_pectoral', l_pec_groups)]
        for bone_name, vertex_groups in pectorals:
            for i in range(0, cuts):
                subbone_name = bone_name + "." + str(i + 1).zfill(3)
                subbone = BODY_RIG.data.edit_bones[subbone_name]
                subbone_name = subbone.name = bone_name + str(i + 1)
                group = bpy.context.object.vertex_groups.new(name=subbone_name)
                vertex_groups.append(group.index)
        BODY_M = self.get_body_mesh()
        bpy.ops.object.select_all(action='DESELECT')
        BODY_M.select_set(True)
        bpy.context.view_layer.objects.active = BODY_M
        bpy.ops.object.mode_set(mode='EDIT')

        def contains_group(vertex, group_index):
            for g in vertex.groups:
                if g.group == group_index:
                    return g.weight
            return 0

        l_pec_idx = BODY_M.vertex_groups['l_pectoral'].index
        r_pec_idx = BODY_M.vertex_groups['r_pectoral'].index
        l_pec_weights = np.array([contains_group(v, l_pec_idx) for v in BODY_M.data.vertices])
        r_pec_weights = np.array([contains_group(v, r_pec_idx) for v in BODY_M.data.vertices])
        for pec_weights, pec_groups in [(l_pec_weights, l_pec_groups), (r_pec_weights, r_pec_groups)]:
            max_weight = np.max(pec_weights)
            for i, subpec_group in enumerate(l_pec_groups):
                subpec_weights = pec_weights - (i + 1) / (cuts + 1)

    def save_textures(self):
        BODY_M = self.get_body_mesh()
        bpy.ops.object.select_all(action='DESELECT')
        BODY_M.select_set(True)
        bpy.context.view_layer.objects.active = BODY_M
        tex_dir = self.textures_dir()
        if os.path.exists(tex_dir):
            shutil.rmtree(tex_dir)
        bpy.ops.daz.save_local_textures()

    def select_gp_or_body(self):
        if 'GoldenPalace_G9 Mesh' in bpy.data.objects:
            mesh = bpy.data.objects['GoldenPalace_G9 Mesh']
        else:
            mesh = self.get_body_mesh()
        bpy.ops.object.select_all(action='DESELECT')
        mesh.select_set(True)
        bpy.context.view_layer.objects.active = mesh
        return mesh

    def unify_golden_palace_uvs(self):

        mesh = self.select_gp_or_body()
        if NEW_GP_UV_MAP not in mesh.data.uv_layers:
            gp_labia_majora = mesh.data.uv_layers['Golden Palace 2']
            gp_labia_minora = mesh.data.uv_layers['Golden Palace']
            gp_labia_majora.active = True
            new_uv_layer = mesh.data.uv_layers.new(name=NEW_GP_UV_MAP)
            new_uv_layer_np = np.array([v.uv for v in new_uv_layer.data])
            gp_labia_majora_np = np.array([v.uv for v in gp_labia_majora.data])
            gp_labia_minora_np = np.array([v.uv for v in gp_labia_minora.data])
            is_majora = np.all(gp_labia_majora_np > 0, axis=1)
            is_minora = np.all(gp_labia_minora_np > 0, axis=1)
            gp_labia_majora_np = np.mod(gp_labia_majora_np, 1)
            gp_labia_minora_np = np.mod(gp_labia_minora_np, 1)
            is_labia_majora = np.logical_and(0.285 < gp_labia_majora_np[:, 0], gp_labia_majora_np[:, 0] < 0.72)
            vagina_symmetry_line = 0.26598
            vagina_extent = 0.47191
            vagina_half_width = vagina_extent - vagina_symmetry_line
            p1A = np.array([0.31444 - vagina_symmetry_line, 0.912306])
            p2A = np.array([0.4164 - vagina_symmetry_line, 0.545495])
            vag_distance = np.absolute(gp_labia_minora_np[:, 0] - vagina_symmetry_line)
            slopeA = (p2A[1] - p1A[1]) / (p2A[0] - p1A[0])
            # p1[2] = p1[0] * slope + offset
            # p1[2] - p1[0] * slope = offset
            offsetA = p1A[1] - p1A[0] * slopeA
            offsetA += 0.05  # just for a good measure to avoid errors due to floating point precision
            p1B = np.array([0.43189 - vagina_symmetry_line, 0.317139])
            p2B = np.array([0.40303 - vagina_symmetry_line, 0.217741])
            slopeB = (p2B[1] - p1B[1]) / (p2B[0] - p1B[0])
            offsetB = p1B[1] - p1B[0] * slopeB
            offsetB -= 0.1  # just for a good measure to avoid errors due to floating point precision
            is_labia_minora = np.logical_and(vag_distance * slopeB + offsetB < gp_labia_minora_np[:, 1],
                                             gp_labia_minora_np[:, 1] < vag_distance * slopeA + offsetA)
            is_labia_majora = np.logical_and(is_labia_majora, np.logical_not(is_labia_minora))
            is_anus = np.logical_and(is_minora, is_majora)
            is_anus = np.logical_and(is_anus, np.logical_not(np.logical_or(is_labia_majora, is_labia_minora)))
            p1 = (0.514551, 0.546842)  # point on circle boundary
            p2 = (0.499947, 0.550254)  # center
            p3 = (0.500005, 0.588271)  # oval top point
            vag_oval_longer_radius = np.linalg.norm(np.subtract(p3, p2))
            vag_radius = np.linalg.norm(np.subtract(p1, p2))
            vag_distance = gp_labia_majora_np - p2
            vag_distance[:, 1] *= vag_radius / vag_oval_longer_radius
            is_vagina = np.linalg.norm(vag_distance, axis=1) < vag_radius
            is_vagina = np.logical_and(is_vagina, np.logical_not(is_labia_minora))
            is_insides = np.logical_or(is_vagina, is_anus)
            new_uv_layer_np[:, :] = 0
            vagina_margin = 0.08
            new_uv_layer_np[is_labia_minora] = gp_labia_minora_np[is_labia_minora] - [vagina_margin, 0]
            new_uv_layer_np[is_labia_majora] = gp_labia_majora_np[is_labia_majora] + [1 - 0.72, 0]
            new_uv_layer_np[is_insides] = gp_labia_minora_np[is_insides] * (1 / 8) + [vagina_half_width * 2 - vagina_margin, 0]
            for v, new_uv in zip(new_uv_layer.data, new_uv_layer_np):
                v.uv = new_uv

    def setup_golden_palace_for_baking(self):
        mesh = self.select_gp_or_body()
        if 'GP_Baked' in bpy.data.images:
            baked_gp_img = bpy.data.images['GP_Baked']
        else:
            baked_gp_img = bpy.data.images.new('GP_Baked', 1024 * 4, 1024 * 4)
        bpy.context.scene.render.engine = 'CYCLES'
        bpy.context.scene.cycles.bake_type = 'DIFFUSE'
        bpy.context.scene.cycles.device = 'GPU'
        bpy.context.scene.render.bake.use_pass_direct = False
        bpy.context.scene.render.bake.use_pass_indirect = False
        mesh.data.uv_layers[NEW_GP_UV_MAP].active = True
        for mat in mesh.data.materials:
            if mat.name.startswith('GP_'):
                n = mat.node_tree.nodes
                l = mat.node_tree.links
                target_texture = n.new('ShaderNodeTexImage')
                target_texture.image = baked_gp_img
                uv_map = n.new('ShaderNodeUVMap')
                uv_map.location = (-300, 200)
                uv_map.uv_map = NEW_GP_UV_MAP

                target_texture.location = (0, 200)
                l.new(target_texture.inputs['Vector'], uv_map.outputs['UV'])
                n.active = target_texture


    # *= 0.25# nails
    # -= 0.5 # nails


def save_textures(duf_filepath):
    duf_filepath = os.path.abspath(duf_filepath)
    workdir = os.path.dirname(duf_filepath)
    name = os.path.basename(duf_filepath)[:-len(".duf")]
    blend_filepath = os.path.join(workdir, name + ".blend")
    bpy.ops.wm.save_as_mainfile(filepath=blend_filepath)
    DazOptimizer(workdir=workdir, name=name).save_textures()


def run_outside_blender():
    import subprocess

    file_path = os.path.realpath(__file__)

    if len(sys.argv) < 2 or not sys.argv[1].endswith(".duf"):
        print("Specify path to .duf")
        exit()
    duf_path: str = os.path.abspath(sys.argv[1])
    if not os.path.exists(duf_path):
        print("File not exists:", duf_path)
        exit()
    subprocess.run(["blender", "-P", file_path, "--", duf_path])

    # Image("")


bl_info = {
    "name": "Daz Optimizer",
    "blender": (2, 80, 0),
    "category": "Object",
}


class EasyImportPanel(bpy.types.Panel):
    bl_label = "Panel"
    bl_idname = "dazoptim_easy_import_panel"
    bl_space_type = 'FILE_BROWSER'
    bl_region_type = 'TOOLS'
    filepath = None
    directory = None

    @classmethod
    def poll(cls, context):
        op = context.active_operator
        if op and op.bl_idname == "DAZ_OT_easy_import_daz":
            cls.directory = op.directory
            cls.filepath = op.filepath
            #context.scene['duf_filepath'] = op.filepath
        return False

    # needs a draw method
    def draw(self, context):
        pass


class DazDelCube_operator(bpy.types.Operator):
    """ Delete default cube """
    bl_idname = "dazoptim.delcube"
    bl_label = "Delete default cube"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return context.mode == "OBJECT"

    def execute(self, context):
        for x in list(bpy.data.objects):
            bpy.data.objects.remove(x)
        for x in list(bpy.data.collections):
            bpy.data.collections.remove(x)
        return {'FINISHED'}


class DazLoad_operator(bpy.types.Operator):
    """ Load daz character """
    bl_idname = "dazoptim.load"
    bl_label = "Load Daz character"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return context.mode == "OBJECT"

    def execute(self, context):
        bpy.ops.daz.easy_import_daz('INVOKE_DEFAULT',
                                    # filepath=self.duf_path,
                                    # files=[filepath],
                                    # directory="",
                                    filter_glob="*.duf;*.dsf;*.png;*.jpeg;*.jpg;*.bmp",
                                    fitMeshes='DBZFILE',
                                    materialMethod='EXTENDED_PRINCIPLED',
                                    useMergeMaterials=True,
                                    useEliminateEmpties=True,
                                    useMergeRigs=False,
                                    useApplyTransforms=False,
                                    useMergeToes=False,
                                    useFavoMorphs=False,
                                    useUnits=False,
                                    useExpressions=False,
                                    useVisemes=False,
                                    useHead=False,
                                    useFacs=False,
                                    useFacsdetails=False,
                                    useFacsexpr=False,
                                    useAnime=False,
                                    usePowerpose=False,
                                    useBody=False,
                                    useBulges=False,
                                    useJcms=True,
                                    ignoreFingers=True,
                                    useMasculine=False,
                                    useFeminine=False,
                                    useFlexions=True,
                                    useBakedCorrectives=True,
                                    useDazFavorites=True,
                                    useAdjusters=False,
                                    onMorphSuffix='SMART',
                                    useTransferFace=True,
                                    useTransferHair=False,
                                    useTransferGeografts=True,
                                    useTransferClothes=True,
                                    useTransferHD=True,
                                    useMergeGeografts=False,
                                    useMakePosable=True,
                                    useFinalOptimization=False,
                                    ignoreUrl=False,
                                    ignoreFinger=False,
                                    morphSuffix="",
                                    ignoreHdMorphs=False,
                                    useMhxOnly=False,
                                    duplicateDistance=1,
                                    useMergeNonConforming='CONTROLS',
                                    useConvertWidgets=True,
                                    useHiddenRigs=False,
                                    useMergeUvs=True,
                                    allowOverlap=False,
                                    keepOriginal=False,
                                    useFixTiles=True,
                                    useSubDDisplacement=True,
                                    useGeoNodes=False,
                                    morphStrength=1,
                                    skinColor=(0.6, 0.4, 0.25, 1),
                                    clothesColor=(0.09, 0.01, 0.015, 1),
                                    useApplyRestPoses=True,
                                    favoPath="")
        return {'FINISHED'}


class DazSave_operator(bpy.types.Operator):
    """ Load daz character """
    bl_idname = "dazoptim.save"
    bl_label = "Load Daz character"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return context.mode == "OBJECT"

    def execute(self, context):
        if bpy.types.dazoptim_easy_import_panel.filepath is None and 'duf_filepath' not in bpy.context.scene:
            self.report({"WARNING"}, "Load a DAZ character first!")
            return {'CANCELLED'}
        if bpy.types.dazoptim_easy_import_panel.filepath is not None and 'duf_filepath' not in bpy.context.scene:
            bpy.context.scene['duf_filepath'] = bpy.types.dazoptim_easy_import_panel.filepath
        save_textures(bpy.context.scene['duf_filepath'])
        return {'FINISHED'}


class DazSimplifyMaterials_operator(bpy.types.Operator):
    """ Simplify materials """
    bl_idname = "dazoptim.simpl_mats"
    bl_label = "Simplify materials"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().simplify_materials()

        return {'FINISHED'}


class DazConcatTextures_operator(bpy.types.Operator):
    """ Concatenate textures into one """
    bl_idname = "dazoptim.concat"
    bl_label = "Concatenate textures"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().concat_textures()

        return {'FINISHED'}


class DazOptimizeEyes_operator(bpy.types.Operator):
    """ Optimize eyes """
    bl_idname = "dazoptim.optim_eyes"
    bl_label = "Optimize eyes"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().optimize_eyes()

        return {'FINISHED'}


class DazMergeGrografts_operator(bpy.types.Operator):
    """ Merge geografts """
    bl_idname = "dazoptim.merge_geografts"
    bl_label = "Merge geografts"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().merge_geografts()

        return {'FINISHED'}


class DazMergeEyes_operator(bpy.types.Operator):
    """ Merge Eyes """
    bl_idname = "dazoptim.merge_eyes"
    bl_label = "Merge eyes"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().merge_eyes()

        return {'FINISHED'}


class DazMergeMouth_operator(bpy.types.Operator):
    """ Merge Mouth """
    bl_idname = "dazoptim.merge_mouth"
    bl_label = "Merge mouth"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().merge_mouth()

        return {'FINISHED'}


class DazOptimizeUVs_operator(bpy.types.Operator):
    """ Optimize UVs """
    bl_idname = "dazoptim.optim_uvs"
    bl_label = "Optimize UVs"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().pack_uvs()

        return {'FINISHED'}


class DazSeparateLipUVs_operator(bpy.types.Operator):
    """ Separate Lip UVs """
    bl_idname = "dazoptim.sep_lip_uvs"
    bl_label = "Separate Lip UVs"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().separate_lip_uvs()

        return {'FINISHED'}


class DazAddBreastBones_operator(bpy.types.Operator):
    """ Add breast bones """
    bl_idname = "dazoptim.breast_bones"
    bl_label = "Optimize UVs"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().subdivide_breast_bones()

        return {'FINISHED'}


class DazAddGluteBones_operator(bpy.types.Operator):
    """ Add glute bones """
    bl_idname = "dazoptim.glute_bones"
    bl_label = "Optimize UVs"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().pack_uvs()

        return {'FINISHED'}


class DazAddThighBones_operator(bpy.types.Operator):
    """ Add thigh bones """
    bl_idname = "dazoptim.thigh_bones"
    bl_label = "Optimize UVs"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().pack_uvs()

        return {'FINISHED'}


class DazFitClothes_operator(bpy.types.Operator):
    """ fit clothes """
    bl_idname = "dazoptim.fit_clothes"
    bl_label = "Fit clothes"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().pack_uvs()

        return {'FINISHED'}


class DazOptimizeHair_operator(bpy.types.Operator):
    """ Optimize hair """
    bl_idname = "dazoptim.optim_hair"
    bl_label = "Optimize Hair"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().pack_uvs()

        return {'FINISHED'}


class DazConvertToUe5Skeleton_operator(bpy.types.Operator):
    """ Convert rig to UE5-compatible skeleton """
    bl_idname = "dazoptim.convert_ue5"
    bl_label = "Convert rig to UE5-compatible skeleton"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().pack_uvs()

        return {'FINISHED'}


class DazOptimizeGoldenPalaceUVs(bpy.types.Operator):
    """ Optimize Golden Palace UVs """
    bl_idname = "dazoptim.optim_gp_uvs"
    bl_label = "Optimize Golden Palace UVs"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().unify_golden_palace_uvs()

        return {'FINISHED'}


class DazSetupGoldenPalaceForBaking(bpy.types.Operator):
    """ Setup Golden Palace for Baking """
    bl_idname = "dazoptim.setup_gp_bake"
    bl_label = "Setup Golden Palace fro baking"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().setup_golden_palace_for_baking()
        return {'FINISHED'}


class DazBakeGoldenPalace(bpy.types.Operator):
    """ Bake Golden Palace """
    bl_idname = "dazoptim.bake_gp"
    bl_label = "Bake Golden Palace"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return context.mode == "OBJECT"

    def execute(self, context):
        bpy.ops.object.bake(type='DIFFUSE')
        return {'FINISHED'}

class DazSendToUnreal(bpy.types.Operator):
    """ Send to unreal """
    bl_idname = "dazoptim.send_to_ue5"
    bl_label = "Send to unreal"
    bl_options = {"REGISTER", "UNDO"}

    @classmethod
    def poll(cls, context):
        return context.mode == "OBJECT"

    def execute(self, context):
        DazOptimizer().pack_uvs()

        return {'FINISHED'}


class DazOptimize_sidebar(bpy.types.Panel):
    """DazOptim actions"""
    bl_label = "DazOptim"
    bl_space_type = "VIEW_3D"
    bl_region_type = "UI"
    bl_category = "DazOptim"

    def draw(self, context):
        col = self.layout.column(align=True)
        i = 0
        for op_class, op_text in operators:
            prop = col.operator(op_class.bl_idname, text=str(i) + ". " + op_text)
            i += 1


operators = [
    (DazDelCube_operator, "Delete default cube"),
    (DazLoad_operator, "Load Daz"),
    (DazSave_operator, "Save textures"),
    (DazSimplifyMaterials_operator, "Simplify materials"),
    (DazOptimizeEyes_operator, "Optimize eyes"),
    (DazOptimizeGoldenPalaceUVs, "Optimize golden palace UVs"),
    (DazSetupGoldenPalaceForBaking, "golden palace prepare baking"),
    (DazBakeGoldenPalace, "Bake golden palace"),
    (DazMergeGrografts_operator, "Merge Geografts"),
    (DazMergeEyes_operator, "Merge eyes"),
    (DazMergeMouth_operator, "Merge mouth"),
    (DazConcatTextures_operator, "Merge textures"),
    (DazOptimizeUVs_operator, "Optimize UVs"),
    (DazSeparateLipUVs_operator, "Separate Lip UVs"),
    (DazAddBreastBones_operator, "Subdivide breast bones"),
    (DazAddGluteBones_operator, "Add glute bones"),
    (DazAddThighBones_operator, "Add thigh bones"),
    (DazFitClothes_operator, "Fit Clothes"),
    (DazOptimizeHair_operator, "Optimize hair"),
    (DazConvertToUe5Skeleton_operator, "Convert to UE5 Skeleton"),
    (DazSendToUnreal, "Send to unreal"),
]

classes = [
              DazOptimize_sidebar,
              EasyImportPanel,
          ] + [op for (op, _) in operators]


def register():
    for c in classes:
        bpy.utils.register_class(c)


def unregister():
    for c in classes:
        bpy.utils.unregister_class(c)


if __name__ == '__main__':
    register()
